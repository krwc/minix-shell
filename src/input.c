#include "input.h"
static Input input;
static char input_buffer[MAX_LINE_LENGTH + 1];
static char line_buffer[MAX_LINE_LENGTH + 1];

ssize_t input_read()
{
    ssize_t num_bytes;
    
    while (true)
    {
        num_bytes = read(STDIN, input_buffer + input.size, MAX_LINE_LENGTH - input.size);
        input.size = input.size == 0 ? num_bytes : input.size + num_bytes;

        if (num_bytes && input.size < MAX_LINE_LENGTH && input_buffer[input.size-1] != '\n')
            continue;
        input_buffer[input.size] = '\0';
        break;
    }
    return num_bytes;
}

bool input_has_line()
{
    for (size_t i = 0; i < input.size; i++)
        if (input_buffer[i] == '\n')
            return true;
    return false;
}

size_t input_copy_line(char* dest)
{
    for (size_t i = 0; i < input.size; i++)
    {
        dest[i] = input_buffer[i];
        if (dest[i] == '\n')
        {
            dest[i + 1] = '\0';
            return i;
        }
    }
    // Should generally not happen.
    return 0;
}

void input_move_input_data(size_t data_start)
{
    static char tmp[MAX_LINE_LENGTH];
    input.size = input.size - data_start;
    
    memset(tmp, 0, MAX_LINE_LENGTH); 
    memcpy(tmp, input_buffer + data_start, input.size);
    memset(input_buffer, 0, MAX_LINE_LENGTH);
    memcpy(input_buffer, tmp, input.size);
}

void input_skip_till_newline()
{
    char ch;
    while (read(STDIN, &ch, 1) > 0 && ch != '\n');

    input.size = 0;
}

const char* input_get_line()
{
    if (input_has_line())
    {
        // Ok, copy this line to the line_buffer.
        size_t line_end = input_copy_line(line_buffer);
        // Now, move the rest of the data from input_buffer
        // to the beginning of it.
        input_move_input_data(line_end + 1);

        return line_buffer;
    }
    else
    {
        // Try to read more characters.
        ssize_t read_ret = input_read();
        if (read_ret)
        {
            if (input_has_line())
                return input_get_line();
            else
            {
                message(STDERR, MSG_SYNTAX_ERROR);
                // Skip characters till you find a newline.
                input_skip_till_newline();
                return input_get_line();
            }
        }
        else
            // End of an input stream.
            return NULL;
    }
}

