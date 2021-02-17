#include "defines.h"
#include "serial.h"

#define SERIAL_SCI_NUM 3

#define H8_3069F_SCI0 ((volatile struct h8_3069f_sci *)0xffffb0)
#define H8_3069F_SCI1 ((volatile struct h8_3069f_sci *)0xffffb8)
#define H8_3069F_SCI2 ((volatile struct h8_3069f_sci *)0xffffc0)

struct h8_3069f_sci {
  volatile uint8 smr;
  volatile uint8 brr;
  volatile uint8 scr;
  volatile uint8 tdr;
  volatile uint8 ssr;
  volatile uint8 rdr;
  volatile uint8 scmr;
};

#define H8_3069F_SCI_SMR_CKS_PER1  (0<<0)
#define H8_3069F_SCI_SMR_CKS_PER4  (1<<0)
#define H8_3069F_SCI_SMR_CKS_PER16 (2<<0)
#define H8_3069F_SCI_SMR_CKS_PER64 (3<<0)
#define H8_3069F_SCI_SMR_MP     (1<<2)
#define H8_3069F_SCI_SMR_STOP   (1<<3)
#define H8_3069F_SCI_SMR_OE     (1<<4)
#define H8_3069F_SCI_SMR_PE     (1<<5)
#define H8_3069F_SCI_SMR_CHR    (1<<6)
#define H8_3069F_SCI_SMR_CA     (1<<7)

#define H8_3069F_SCI_SCR_SKE0   (1<<0)
#define H8_3069F_SCI_SCR_SKE1   (1<<1)
#define H8_3069F_SCI_SCR_TEIE   (1<<2)
#define H8_3069F_SCI_SCR_MPIE   (1<<3)
#define H8_3069F_SCI_SCR_RE     (1<<4) // receive enabled
#define H8_3069F_SCI_SCR_TE     (1<<5) // transmit enabled
#define H8_3069F_SCI_SCR_RIE    (1<<6) // receive interrupt enabled
#define H8_3069F_SCI_SCR_TIE    (1<<7) // transmit interrupt enabled

#define H8_3069F_SCI_SSR_MPBT   (1<<0)
#define H8_3069F_SCI_SSR_MPB    (1<<1)
#define H8_3069F_SCI_SSR_TEND   (1<<2)
#define H8_3069F_SCI_SSR_PER    (1<<3)
#define H8_3069F_SCI_SSR_FERERS (1<<4)
#define H8_3069F_SCI_SSR_ORER   (1<<5)
#define H8_3069F_SCI_SSR_RDRF   (1<<6) // receive completed
#define H8_3069F_SCI_SSR_TDRE   (1<<7) // transmit completed

static struct {
  volatile struct h8_3069f_sci *sci;
} regs[SERIAL_SCI_NUM] = {
  {H8_3069F_SCI0},
  {H8_3069F_SCI1},
  {H8_3069F_SCI2},
};

// initialize device
int serial_init(int index)
{
  volatile struct h8_3069f_sci *sci = regs[index].sci;

  sci->scr = 0;
  sci->smr = 0;
  sci->brr = 64; // generate 9600bsp from 20MHz clock (if 25HMz use 80).
  sci->scr = H8_3069F_SCI_SCR_RE | H8_3069F_SCI_SCR_TE; // recv/tran enabled.
  sci->ssr = 0;

  return 0;
}


// is transmit enabled or not
int serial_is_send_enabled(int index)
{
  volatile struct h8_3069f_sci *sci = regs[index].sci;

  return (sci->ssr & H8_3069F_SCI_SSR_TDRE);
}


// transmit 1 character
int serial_send_byte(int index, unsigned char c)
{
  volatile struct h8_3069f_sci *sci = regs[index].sci;

  // wait until transmit enabled
  while (!serial_is_send_enabled(index))
    ;

  sci->tdr = c;
  sci->ssr &= ~H8_3069F_SCI_SSR_TDRE; // transmit start

  return 0;
}
