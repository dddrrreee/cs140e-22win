// register numbers.  p57
//      name ,      reg number  ,   initial value,  size
NRF_REG(CONFIG,         0x0,        0b1000,         1)
NRF_REG(EN_RXADDR,      0x2,        0b11,           1)
NRF_REG(EN_AA,          0x1,        0b111111,       1)
NRF_REG(SETUP_AW,       0x3,        0b11,           1)
NRF_REG(SETUP_RETR,     0x4,        0b11,           1)
NRF_REG(RF_CH,          0x5,        0b10,           1)
// this one has a don't care on the lowest bit --- on our chips seems to be 1.
NRF_REG(RF_SETUP,       0x6,        0b1111,         1)
NRF_REG(STATUS,         0x7,        0b1110,         1)
NRF_REG(RPD,            0x9,        0,              1)

// wait: we don't write to this, what is up?
NRF_REG(RX_ADDR_P0,     0xa,        0xe7e7e7e7e7ULL,    5)
NRF_REG(RX_ADDR_P1,     0xb,        0xc2c2c2c2c2ULL,    5)
NRF_REG(RX_ADDR_P2,     0xc,        0xc3,               1)
NRF_REG(RX_ADDR_P3,     0xd,        0xc4,               1)
NRF_REG(RX_ADDR_P4,     0xe,        0xc5,               1)
NRF_REG(RX_ADDR_P5,     0xf,        0xc6,               1)

NRF_REG(TX_ADDR,        0x10,       0xe7e7e7e7e7ULL,    5)

NRF_REG(RX_PW_P0,       0x11,       0,                  1)
NRF_REG(RX_PW_P1,       0x12,       0,                  1)
NRF_REG(RX_PW_P2,       0x13,       0,                  1)
NRF_REG(RX_PW_P3,       0x14,       0,                  1)
NRF_REG(RX_PW_P4,       0x15,       0,                  1)
NRF_REG(RX_PW_P5,       0x16,       0,                  1)
NRF_REG(FIFO_STATUS,    0x17,       0b10001,            1)
NRF_REG(DYNPD,          0x1c,       0,                  1)
NRF_REG(FEATURE,        0x1d,       0,                  1)
