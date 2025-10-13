#ifndef COMMUNICATE_H
#define COMMUNICATE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define COMMUNICATE_BUFFER_SIZE 256
#define COMMUNICATE_LINE_ENDING "\r\n"
#define COMMUNICATE_LINE_ENDING_LENGTH 2
#define COMMUNICATE_START_CHAR '>'

/**
 * @brief Communication interface structure
 * Start character: '>'
 * Length: 2 bytes
 * Data: variable length
 * Checksum: 2 bytes (CRC16)
 * End characters: "\r\n"
 */

typedef struct Communicate
{
    uint8_t buffer[COMMUNICATE_BUFFER_SIZE];
    uint16_t buffer_length;
    uint16_t buffer_index;
    
} Communicate_t;

void communicate_init(Communicate_t *comm);
void communicate_receive_char(Communicate_t *comm, uint8_t ch);
void communicate_printf(Communicate_t *comm, const char *format, ...);
void communicate_put_line(Communicate_t *comm, const char *line);


#ifdef __cplusplus
}
#endif
#endif /* COMMUNICATE_H */