#include <zephyr/sys/ring_buffer.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include <stdio.h>

//Criando e inicializando o ring_buffer
#define RING_BUF_SIZE 128
RING_BUF_DECLARE(ring_buf, RING_BUF_SIZE);

// inserir dados no ring buffer
void ring_buf_write(uint8_t *data, uint32_t size) {
    uint32_t bytes_written = ring_buf_put(&ring_buf, data, size);

    if (bytes_written != size) {
        printf("Falha ao inserir todos os dados no ring buffer.\n");
    } else {
        printf("Dados inseridos com sucesso no ring buffer.\n");
    }
}

// ler dados do ring buffer
void ring_buf_read(uint8_t *data, uint32_t size) {
    uint32_t bytes_read = ring_buf_get(&ring_buf, data, size);

    if (bytes_read == 0) {
        printf("Ring buffer está vazio.\n");
    } else {
        printf("Dados lidos do ring buffer: %d bytes\n", bytes_read);
    }
}


void main() {
    uint8_t write_data[] = "Teste de ring buffer";
    uint8_t read_data[sizeof(write_data)];

    ring_buf_write(write_data, sizeof(write_data)); //put
    uint32_t bytes_read = ring_buf_peek(&ring_buf,write_data, sizeof(read_data)); //peek
    ring_buf_read(read_data, sizeof(read_data)); //get

    ring_buf_reset(&ring_buf); // clear
    
    printf("Dados lidos: ");
    for (int i = 0; i < sizeof(read_data); i++) {
        printf("%c", read_data[i]);
    }
    printf("\n");
    printf("%d", bytes_read);

    return 0;
}
