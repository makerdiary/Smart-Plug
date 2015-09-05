// rftransceive.h - RF transceiver
// 2010 feb 23  v4  Johan vd Graaf    Added defines for return values
// 2010 feb 19  v3  Johan vd Graaf    New API
// 2010 jan 15  v2  Ruud Hendricksen  Ported to HBL
// 2009 dec 31  v1  Johan vd Graaf    Created

#ifndef _RFTRANSCEIVE_H_
#define _RFTRANSCEIVE_H_

// Return values for RF API
#define RF_SUCCESS                   0   // API function ended successfully
#define RF_RX_ABORT                  -1  // read function was aborted by a transmit
#define RF_RX_TIMEOUT                -2  // read function was aborted by a timeout
#define RF_CALLING_ERROR             -3  // API function calling is not according specification
#define RF_COMMUNICATION_ERROR       -4  // init function failed because HW interface is broken
#define RF_ENCODING_UNKNOWN          -5
#define RF_RUN_IN_CHIP_CNT_TOO_SMALL -6
#define RF_PREAMBLE_CHIP_CNT_INVALID -7

// Maximum RF message length (for transmit and thus also for receive)
#ifdef SUPPORT_MAX_MBUS_MSG_SIZE
#define RF_MAX_RXTX_MESSAGE_LENGTH   290
#else
#define RF_MAX_RXTX_MESSAGE_LENGTH   80
#endif

#define RF_FALSE                     0
#define RF_TRUE                      1

#define RF_INIT_WARM                 0
#define RF_INIT_COLD                 1

#define RF_MODULATION_FSK            0
#define RF_MODULATION_ASK            1

#define RF_ENCODING_NRZ              0
#define RF_ENCODING_MANCHESTER_HW    1
#define RF_ENCODING_MANCHESTER_SW    2   // to be able to send a non-manchester "chip sequence" as Start of Msg
#define RF_ENCODING_3_OUT_OF_6       3

#define RF_OUTPUT_POWER_MIN          1
#define RF_OUTPUT_POWER_MAX          31

#define RF_RX_GAIN_LOW               0
#define RF_RX_GAIN_HIGH              1

// Poll wakeup time between RF_MIN_WAKEUP_TIME and RF_MAX_WAKEUP_TIME
// From 1ms till 256ms in 1ms steps, above 256ms till 4096ms in 16ms steps
#define RF_WAKEUP_TIME_MIN           1
#define RF_WAKEUP_TIME_MAX           4096

// supported chiprates
#define RF_CHIPRATE_4800             1
#define RF_CHIPRATE_32768            2
#define RF_CHIPRATE_100000           3

typedef struct rf_tx_setup_s
{
    unsigned int    run_in_chip_cnt;     // Used for transmitter clock sync in receiver and for polled RF detection
    unsigned int    preamble_chip_cnt;   // minimal 1 chip and maximal 32 chips
    unsigned int    preamble_value;      // the preamble bits themselves
    unsigned char   rf_modulation;       // RF_MODULATION_FSK or RF_MODULATION_ASK
    unsigned char   rf_encoding;         // RF_ENCODING_NRZ, RF_ENCODING_MANCHESTER, RF_ENCODING_3_OUT_OF_6
    unsigned char   rf_output_power;     // output power between RF_OUTPUT_POWER_MIN and RF_OUTPUT_POWER_MAX
    unsigned char   rf_chiprate;
    unsigned char   rf_low_freq_is_zero; // Boolean flag to select ZERO modulation frequency
} rf_tx_setup_t;

typedef struct rf_rx_setup_s
{
    unsigned int    preamble_chip_cnt;   // minimal 1 chip and maximal 32 chips
    unsigned int    preamble_value;      // the preamble bits themselves
    unsigned char   rf_modulation;       // RF_MODULATION_FSK or RF_MODULATION_ASK
    unsigned char   rf_encoding;         // RF_ENCODING_NRZ, RF_ENCODING_MANCHESTER, RF_ENCODING_3_OUT_OF_6
    unsigned char   rf_rx_gain;          // RF_RX_GAIN_LOW or RF_RX_GAIN_HIGH
    unsigned char   rf_chiprate;
    unsigned char   rf_low_freq_is_zero; // Boolean flag to select ZERO modulation frequency
} rf_rx_setup_t;

int rf_init       ( int cold_init );
int rf_write      ( char * buf, int size, rf_tx_setup_t * tx_setup );
int rf_start_read ( rf_rx_setup_t * rx_setup );
int rf_read       ( char * buf, int size, unsigned int timeout );
int rf_stop_read  ( int * rssi_level );

#endif
