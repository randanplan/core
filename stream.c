/*
  stream.h - stream RX handling for tool change protocol

  Part of grblHAL

  Copyright (c) 2021 Terje Io

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>

#include "hal.h"
#include "protocol.h"
#include "state_machine.h"

static stream_rx_buffer_t rxbackup;

typedef struct {
    enqueue_realtime_command_ptr enqueue_realtime_command;
    stream_read_ptr read;
    stream_rx_buffer_t *rxbuffer;
} stream_state_t;

static stream_state_t stream = {0};

// "dummy" version of serialGetC
int16_t stream_get_null (void)
{
    return SERIAL_NO_DATA;
}

static bool await_toolchange_ack (char c)
{
    if(c == CMD_TOOL_ACK && !stream.rxbuffer->backup) {
        memcpy(&rxbackup, stream.rxbuffer, sizeof(stream_rx_buffer_t));
        stream.rxbuffer->backup = true;
        stream.rxbuffer->tail = stream.rxbuffer->head;
        hal.stream.read = stream.read; // restore normal input
        hal.stream.set_enqueue_rt_handler(stream.enqueue_realtime_command);
        stream.enqueue_realtime_command = NULL;
    } else
        return stream.enqueue_realtime_command(c);

    return true;
}

bool stream_rx_suspend (stream_rx_buffer_t *rxbuffer, bool suspend)
{
    if(suspend) {
        stream.rxbuffer = rxbuffer;
        stream.read = hal.stream.read;
        stream.enqueue_realtime_command = hal.stream.set_enqueue_rt_handler(await_toolchange_ack);
        hal.stream.read = stream_get_null;
    } else {
        if(rxbuffer->backup)
            memcpy(rxbuffer, &rxbackup, sizeof(stream_rx_buffer_t));
        if(stream.enqueue_realtime_command) {
            hal.stream.read = stream.read; // restore normal input
            hal.stream.set_enqueue_rt_handler(stream.enqueue_realtime_command);
            stream.enqueue_realtime_command = NULL;
        }
    }

    return rxbuffer->tail != rxbuffer->head;
}

ISR_CODE bool stream_buffer_all (char c)
{
    return false;
}

ISR_CODE bool stream_enable_mpg (const io_stream_t *mpg_stream, bool mpg_mode)
{
    static io_stream_t org_stream = {
        .type = StreamType_Redirected
    };

    sys_state_t state = state_get();

    // Deny entering MPG mode if busy
    if(mpg_mode == sys.mpg_mode || (mpg_mode && (gc_state.file_run || !(state == STATE_IDLE || (state & (STATE_ALARM|STATE_ESTOP)))))) {
        protocol_enqueue_realtime_command(CMD_STATUS_REPORT_ALL);
        return false;
    }

    if(mpg_mode) {
        if(org_stream.type == StreamType_Redirected) {
            memcpy(&org_stream, &hal.stream, sizeof(io_stream_t));
            if(hal.stream.disable)
                hal.stream.disable(true);
            mpg_stream->disable(false);
            mpg_stream->set_enqueue_rt_handler(org_stream.set_enqueue_rt_handler(NULL));
            hal.stream.read = mpg_stream->read;
            hal.stream.get_rx_buffer_free = mpg_stream->get_rx_buffer_free;
            hal.stream.cancel_read_buffer = mpg_stream->cancel_read_buffer;
            hal.stream.reset_read_buffer = mpg_stream->reset_read_buffer;
        }
    } else if(org_stream.type != StreamType_Redirected) {
        mpg_stream->disable(true);
        memcpy(&hal.stream, &org_stream, sizeof(io_stream_t));
        org_stream.type = StreamType_Redirected;
        if(hal.stream.disable)
            hal.stream.disable(false);
    }

    hal.stream.reset_read_buffer();

    sys.mpg_mode = mpg_mode;
    sys.report.mpg_mode = On;

    // Force a realtime status report, all reports when MPG mode active
    protocol_enqueue_realtime_command(mpg_mode ? CMD_STATUS_REPORT_ALL : CMD_STATUS_REPORT);

    return true;
}

#ifdef DEBUGOUT

static stream_write_ptr dbg_write;

static void debug_write (const char *s)
{
    dbg_write(s);
    while(hal.debug.get_tx_buffer_count()); // Wait until message is delivered
}

void debug_stream_init (io_stream_t *stream)
{
    memcpy(&hal.debug, stream, sizeof(io_stream_t));
    dbg_write = hal.debug.write;
    hal.debug.write = debug_write;

    hal.debug.write(ASCII_EOL "UART debug active:" ASCII_EOL);
}

#endif
