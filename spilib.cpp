#include <Arduino.h>
#include <SPI.h>
#include <string.h>

#include "frser-cfg.h"
#include "frser-flashapi.h"

#include "spilib.h"
#include "uart.h"

static uint8_t spi_initialized = 0;

#ifdef FRSER_FEAT_SPISPEED
static uint32_t spi_current_hz = 8000000; // default max for Uno

#ifdef __cplusplus
extern "C" {
#endif

uint32_t spi_set_speed(uint32_t hz) {
    // Arduino SPI supports arbitrary clock (within limits)
    spi_current_hz = hz;

    if (spi_initialized) {
        SPI.end();
        SPI.begin();
    }
    return spi_current_hz;
}

#endif

static void frser_spi_init(void) {
    SPI.begin();

    // Configure SPI (equivalent to your SPCR setup)
    SPI.beginTransaction(SPISettings(
#ifdef FRSER_FEAT_SPISPEED
        spi_current_hz,
#else
        8000000, // default max speed for 16 MHz AVR
#endif
        MSBFIRST,
        SPI_MODE0
    ));

    spi_initialized = 1;
}

static uint8_t spi_uninit(void) {
    if (spi_initialized) {
        SPI.end();
        spi_initialized = 0;
        return 1;
    }
    return 0;
}

// --------------------------------------------------
// Basic SPI transaction helper
// --------------------------------------------------

static inline uint8_t spi_xfer(uint8_t v) {
    return SPI.transfer(v);
}

// --------------------------------------------------
// Generic SPI operations
// --------------------------------------------------

static void spi_localop_start(uint8_t sbytes, const uint8_t* sarr) {
    spi_select();
    for (uint8_t i = 0; i < sbytes; i++) {
        spi_xfer(sarr[i]);
    }
}

static void spi_localop_end(uint8_t rbytes, uint8_t* rarr) {
    for (uint8_t i = 0; i < rbytes; i++) {
        rarr[i] = spi_xfer(0xFF);
    }
    spi_deselect();
}

static void spi_localop(uint8_t sbytes, const uint8_t* sarr,
                        uint8_t rbytes, uint8_t* rarr) {
    spi_localop_start(sbytes, sarr);
    spi_localop_end(rbytes, rarr);
}

// --------------------------------------------------
// Stream-based SPI (no AVR async model)
// --------------------------------------------------

static void spi_spiop_start(uint32_t sbytes) {
    spi_select();
    while (sbytes--) {
        spi_xfer(RECEIVE());
    }
}

static void spi_spiop_end(uint32_t rbytes) {
    while (rbytes--) {
        SEND(spi_xfer(0xFF));
    }
    spi_deselect();
}

// --------------------------------------------------
// Combined operation
// --------------------------------------------------

void spi_spiop(uint32_t sbytes, uint32_t rbytes) {
    spi_spiop_start(sbytes);
    SEND(S_ACK);
    spi_spiop_end(rbytes);
}

// --------------------------------------------------
// Utility
// --------------------------------------------------

uint8_t oddparity(uint8_t val) {
    val = (val ^ (val >> 4)) & 0x0F;
    val = (val ^ (val >> 2)) & 0x03;
    return (val ^ (val >> 1)) & 0x01;
}

// --------------------------------------------------
// SPI Flash ID probes
// --------------------------------------------------

uint8_t spi_probe_rdid(uint8_t *id) {
    const uint8_t cmd = 0x9F;
    uint8_t in[3];

    spi_localop(1, &cmd, 3, in);

    if (!oddparity(in[0])) return 0;
    if ((in[0] == 0xFF && in[1] == 0xFF && in[2] == 0xFF)) return 0;
    if ((in[0] == 0x00 && in[1] == 0x00 && in[2] == 0x00)) return 0;

    if (id) memcpy(id, in, 3);
    return 1;
}

uint8_t spi_probe_rems(uint8_t *id) {
    const uint8_t cmd[4] = {0x90, 0, 0, 0};
    uint8_t in[2];

    spi_localop(4, cmd, 2, in);

    if ((in[0] == 0xFF && in[1] == 0xFF)) return 0;
    if ((in[0] == 0x00 && in[1] == 0x00)) return 0;

    if (id) memcpy(id, in, 2);
    return 1;
}

uint8_t spi_probe_res(uint8_t *id) {
    const uint8_t cmd[4] = {0xAB, 0, 0, 0};
    uint8_t in[1];

    spi_localop(4, cmd, 1, in);

    if (in[0] == 0xFF || in[0] == 0x00) return 0;

    if (id) *id = in[0];
    return 1;
}

// --------------------------------------------------
// Test
// --------------------------------------------------

uint8_t spi_test(void) {
    frser_spi_init();
    if (spi_probe_rdid(NULL)) return 1;
    if (spi_probe_rems(NULL)) return 1;
    if (spi_probe_res(NULL)) return 1;
    spi_uninit();
    return 0;
}

// --------------------------------------------------
// Read operations
// --------------------------------------------------

uint8_t spi_read(uint32_t addr) {
    uint8_t cmd[4] = {
        0x03,
        (uint8_t)(addr >> 16),
        (uint8_t)(addr >> 8),
        (uint8_t)(addr)
    };

    uint8_t r;
    spi_localop(4, cmd, 1, &r);
    return r;
}

void spi_readn(uint32_t addr, uint32_t len) {
    uint8_t cmd[4] = {
        0x03,
        (uint8_t)(addr >> 16),
        (uint8_t)(addr >> 8),
        (uint8_t)(addr)
    };

    spi_localop_start(4, cmd);

    while (len--) {
        SEND(spi_xfer(0xFF));
    }

    spi_deselect();
}

void flash_set_safe(void) {
	spi_uninit();
}

void flash_select_protocol(uint8_t allowed_protocols) {
	(void)allowed_protocols;

	frser_spi_init();
}

void flash_spiop(uint32_t s, uint32_t r) {
	spi_spiop(s,r);
}

#ifdef __cplusplus
}
#endif