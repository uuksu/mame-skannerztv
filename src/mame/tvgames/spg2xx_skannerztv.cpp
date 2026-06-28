// license:BSD-3-Clause
// copyright-holders:Ryan Holtz, David Haywood

// Skannerz TV

/* I/O related notes from Tahg

Summary:
SEND(from console)              RECEIVE (to console)
--- Load monsters / items
1                               2
128 + 0                         M/I[0] HIBYTE, LOBYTE
...                             ...
128 + 99                        M/I[99] HIBYTE, LOBYTE
--- Save monsters / items
4                               5
128 + 0, M/I[0] HIBYTE, LOBYTE  6
...                             ...
128 + 99, M/I[99] HIBYTE, LOBYTE 6
--- Buttons
3                               BHI, BLO

Buttons
BLO:
Bit 0     Up
Bit 1     Down
Bit 2     2/B
Bit 3     1/A
Bit 4     Left
Bit 5     Right
Bit 6     Unused
Bit 7     Unused
BHI:      Unused

Port A
Bit 0     Enable "Left" Controller (Scanner 1 / Player 1)
Bit 1     Enable "Right" Controller (Scanner 2 / Player 2)

Player
0:
8:
B:
D:
E:
65:  monsters
B5:  items

doPlayer(player)
     var_3 = player - 1
     pData = Player[var_3]
     switch(PlayerState[var_3])
          case 0: // Request controller -> console state
               r1 = 79
               while(--r1 >= 0)
               PlayerState[var_3] = 1
               P_UART_Ctrl |= TxEn
               P_UART_Ctrl |= TxIntEn
               while(P_UART_Status & TXBUSY)
               P_UART_Ctrl &= ~RxEn
               P_UART_TxBuf = 1
               WaitCycle[var_3] = 6
               CurMI[var_3] = 0
               break
          case 1: // Receive controller -> console state (expects 2)
               if(Uartp != Uartg)
                    byte_1241 = UARTBuffer[Uartg]
                    if(byte_1241 == 2)
                         PlayerState[var_3] = 2
                         InitPlayer(var_3)
                    Uartg = Uartg == 7 ? 0 : Uartg + 1
               break
          case 2: // Sync monster / item
               if(CurMI[var_3] != 100)
                    var_6 = CurMI[var_3] | 80h
                    while(P_UART_Status & TXBUSY)
                    P_UART_Ctrl &= ~RxEn
                    P_UART_TxBuf = var_6
                    PlayerState[var_3] = 3
                    WaitCycle[var_3] = 6
                    Retries[var_3] = 3
                    CurRecByte[var_3] = 0
               else if(player == 1 && byte_1238 & 100h)
                    PlayerState[var_3] = 6
                    byte_1238 &= ~100h
               else if(player == 2 && byte_1238 & 200h)
                    PlayerState[var_3] = 6
                    byte_1238 &= ~200h
               else
                    PlayerState[var_3] = 4
                    if(player == 1)
                         P_IOA_Buffer |= 1
                         byte_1238 |= 11
                    else if(player == 2)
                         P_IOA_Buffer |= 2
                         byte_1238 |= 22
               break
          case 3: // Receive monster / item
               if(Uartp != Uartg)
                    r4 = CurMI[var_3]
                    var_A = UARTBuffer[Uartg]
                    if(CurRecByte[var_3] == 0)
                         if(r4 > 79)
                              pData.items[r4-80] = var_A << 8
                         else pData.monsters[r4] = var_A << 8
                         CurRecByte[var_3] += 1
                    else if(CurRecByte[var_3] == 1)
                         if(r4 > 79)
                              pData.items[r4-80] |= var_A
                         else pData.monsters[r4] |= var_A
                         PlayerState[var_3] = 2
                         CurMI[var_3] += 1
                    Uartg = Uartg == 7 ? 0 : Uartg + 1
               break
          case 4:
               if(player == 1)
                    if((byte_1238 & 4) == 0)
                         byte_1238 |= 4
                         r1 = 79
                         while(--r1 >= 0)
                         while(P_UART_Status & TXBUSY)
                    else break
               else if(player == 2)
                    if((byte_1238 & 8) == 0)
                         byte_1238 |= 8
                         r1 = 79
                         while(--r1 >= 0)
                         while(P_UART_Status & TXBUSY)
                         break
                    else break
               else break
               P_UART_Ctrl &= ~RxEn
               P_UART_TxBuf = 3
               PlayerState[var_3] = 5
               WaitCycle[var_3] = 6
               Retries[var_3] = 3
               CurRecByte[var_3] = 0
               break
          case 5:
               if(Uartp != Uartg)
                    var_4 = CurRecByte[var_3]
                    if(var_4 == 0)
                         byte_1242[var_3] = UARTBuffer[Uartg] & 3fh
                         CurRecByte[var_3] += 1
                    else if(var_4 == 1)
                         byte_1234[var_3] = UARTBuffer[Uartg] & 3fh
                         PlayerState = 2
                         WaitCycle[var_3] = 0
                         if(player == 1)
                              P_IOA_Buffer |= 1
                              byte_1238 &= ~4
                         else if(player == 2)
                              P_IOA_Buffer |= 2
                              byte_1238 &= ~8
                    Uartg = Uartg == 7 ? 0 : Uartg + 1
               break:
          case 6: // Request console -> controller state
               r1 = 79
               while(--r1 >= 0)
               while(P_UART_Status & TXBUSY)
               P_UART_Ctrl &= ~RxEn
               P_UART_TxBuf = 4
               PlayerState[var_3] = 7
               WaitCycle[var_3] = 6
               CurMI[var_3] = 0
               Retries[var_3] = 3
          case 7: // Receive console -> controller state (expects 5)
               if(Uartp != Uartg)
                    if(UARTBuffer[Uartg] == 5)
                         PlayerState = 8
                         WaitCycle[var_3] = 6
                         CurSendByte[var_3] = 0
                         Retries[var_3] = 3
                    Uartg = Uartg == 7 ? 0 : Uartg + 1
               break
          case 8: // Send monster / item
               r4 = CurMI[var_3]
               if( r4 <= 99)
                    var_9 = CurSendByte[var_3]
                    var_8 = 0
                    if((P_UART_Status & TXBUSY) == 0)
                         var_8 = 1
                    if(var_8)
                         if(var_9 == 0)
                              var_1 = r4 | 80h
                              while(P_UART_Status & TXBUSY)
                              P_UART_Ctrl &= ~RxEn
                              P_UART_TxBuf = var_1
                              WaitCycle[var_3] = 6
                              CurSendByte[var_3] += 1
                         else if(var_9 == 1)
                              if (r4 <= 79)
                                   var_1 = (pData.monsters[r4] & 0xFF00) >> 8
                                   while(P_UART_Status & TXBUSY)
                                   P_UART_Ctrl &= ~RxEn
                                   P_UART_TxBuf = var_1
                              else
                                   var_1 = (pData.monsters[r4-80] & 0xFF00) >> 8
                                   while(P_UART_Status & TXBUSY)
                                   P_UART_Ctrl &= ~RxEn
                                   P_UART_TxBuf = var_1
                              WaitCycle[var_3] = 6
                              CurSendByte[var_3] += 1
                         else if(var_9 == 2)
                              if (r4 <= 79)
                                   var_1 = pData.monsters[r4] & 0xFF
                                   while(P_UART_Status & TXBUSY)
                                   P_UART_Ctrl &= ~RxEn
                                   P_UART_TxBuf = var_1
                              else
                                   var_1 = pData.monsters[r4-80] & 0xFF
                                   while(P_UART_Status & TXBUSY)
                                   P_UART_Ctrl &= ~RxEn
                                   P_UART_TxBuf = var_1
                              WaitCycle[var_3] = 6
                              CurSendByte[var_3] += 1
                              PlayerState[var_3] = 9
               else
                    PlayerState = 4
                    if(player == 1)
                         P_IOA_Buffer |= 1
                         byte_1238 |= 4
                    else if(player == 2)
                         P_IOA_Buffer |= 2
                         byte_1238 |= 8
               break
          case 9: // Receive monster / item sent state (expects 6)
               if(Uartp != Uartg)
                    if(UARTBuffer[Uartg] == 6)
                         PlayerState[var_3] = 8
                         WaitCycle[var_3] = 0
                         CurSendByte[var_3] = 0
                         CurMI[var_3] += 1
                         Retries[var_3] = 3
               break
     doPlayerMore(player)

doPlayerMore(player)
     var_2 = WaitCycle[player-1]
     if(var_2 == 0) return
     var_2--
     WaitCycle[player-1] = var_2
     if(var_2 != 0) return
     switch(PlayerState[player-1])
          case 3:
               if(Retries[player-1]--)
                    var_3 = CurMI[player-1] | 80h
                    while(P_UART_Status & TXBUSY)
                    P_UART_Ctrl = var_1 = P_UART_Ctrl & ~RxEn
                    P_UART_TxBuf = var_3
                    WaitCycle[player-1] = 6
                    CurRecByte[player-1] = 0
                    return
               break
          case 5:
               if(Retries[player-1]--)
                    while(P_UART_Status & TXBUSY)
                    P_UART_Ctrl = var_1 = P_UART_Ctrl & ~RxEn
                    P_UART_TxBuf = 3
                    WaitCycle[player-1] = 6
                    CurRecByte[player-1] = 0
                    return
               break
          case 7:
               if(Retries[player-1]--)
                    while(P_UART_Status & TXBUSY)
                    P_UART_Ctrl = var_1 = P_UART_Ctrl & ~RxEn
                    P_UART_TxBuf = 4
                    WaitCycle[player-1] = 6
                    return
               break
          case 9:
               if(Retries[player-1]--)
                    PlayerState = 8
                    CurSendByte[player] = 0
               else
                    ClearPlayer(player)
               return
          case 1:
               ClearPlayer(player)
               return
     ClearPlayer(player)
     if(player == 1)
          byte_1238 |= 40h
          byte_1238 &= ~10h
     else if(player == 2)
          byte_1238 |= 80h
          byte_1238 &= ~20h

*/

#include "emu.h"
#include "spg2xx.h"
#include "machine/nvram.h"


namespace {

class skannerztv_state : public spg2xx_game_state
{
public:
	skannerztv_state(const machine_config &mconfig, device_type type, const char *tag) :
		spg2xx_game_state(mconfig, type, tag),
		m_io_scan1(*this, "SCAN1"),
		m_io_scan2(*this, "SCAN2"),
		m_porta_data(0),
		m_vsk_state(0),
		m_vsk_mi(0),
		m_save_byte(0),
		m_byte2_timer(nullptr),
		m_byte2_pending(0)
	{
	}

	void rad_sktv(machine_config& config);

private:
	optional_ioport m_io_scan1;
	optional_ioport m_io_scan2;

	uint16_t m_porta_data;

	// Single virtual scanner state machine (one scanner emulated, not two).
	// Both firmware players probe simultaneously with the same IOA, so the
	// winning player absorbs our single probe ACK; the other keeps probing.
	// We ignore all probes once M/I loading starts to prevent FIFO contamination.
	//   0 = awaiting probe (tx 0x01)
	//   1 = loading monsters/items (tx 0x80..0xe3, 100 pairs of 2 rx bytes each)
	//   2 = button polling (tx 0x03) or save trigger (tx 0x04)
	//   3 = save in progress: firmware sends 3 bytes per slot (index|0x80, HI, LO)
	//       scanner ACKs the LO byte with 0x06; repeats 100 times
	int m_vsk_state;
	int m_vsk_mi;

	// Byte counter within the current 3-byte save slot sequence (0=index, 1=HI, 2=LO).
	int m_save_byte;

	// Deduplication: doPlayer(1) and doPlayer(2) run in the same interrupt tick
	// and may both write TXBUF=0x01 with the same IOA.  Record when we last
	// emitted a probe ACK so the second write in the same tick is silently ignored.
	attotime m_last_probe_time;

	// Delayed second-byte delivery: uart_rx_force fires the baud-rate timer after
	// the first byte, but WaitCycle (~6 ms) is shorter than one baud period (~17 ms).
	// We push the second byte ourselves after a short delay (well under 6 ms).
	emu_timer *m_byte2_timer;
	uint8_t m_byte2_pending;

	// Monster/item type IDs returned during M/I load.  Each entry is the 16-bit
	// slot value (HI<<8|LO) that becomes pData.monsters[n] or pData.items[n-80].
	// The game indexes the ROM monster table at 0xe198 (8 words/entry) with this
	// value; entries listed here are verified non-empty in the ROM table.
	// Slots  0-79: monster types; slots 80-99: item types.
	static const uint16_t s_mi_cheat[100];

	void porta_out_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	void uart_txbuf_tap(offs_t offset, uint16_t &data, uint16_t mem_mask);
	TIMER_CALLBACK_MEMBER(byte2_tick);

	void machine_start() override ATTR_COLD;
	void machine_reset() override ATTR_COLD;
};

void skannerztv_state::machine_start()
{
	spg2xx_game_state::machine_start();
	save_item(NAME(m_porta_data));
	save_item(NAME(m_vsk_state));
	save_item(NAME(m_vsk_mi));
	save_item(NAME(m_save_byte));
	save_item(NAME(m_byte2_pending));

	m_byte2_timer = timer_alloc(FUNC(skannerztv_state::byte2_tick), this);

	// Tap UART TXBUF writes (word addr 0x3d35) so we can reply to the scanner
	// protocol before the baud timer fires — the firmware's WaitCycle timeout
	// (6 ticks @ ~1 kHz = ~6 ms) is shorter than one 600-baud bit period (~17 ms).
	m_maincpu->space(AS_PROGRAM).install_write_tap(
		0x3d35, 0x3d35, "skz_uart_txbuf",
		[this](offs_t offset, uint16_t &data, uint16_t mem_mask)
		{
			uart_txbuf_tap(offset, data, mem_mask);
		});
}

void skannerztv_state::machine_reset()
{
	spg2xx_game_state::machine_reset();
	m_porta_data = 0;
	m_vsk_state = 0;
	m_vsk_mi = 0;
	m_save_byte = 0;
	m_byte2_pending = 0;
	m_last_probe_time = attotime::never;
}

// Track IOA writes: bit 0 selects scanner 1, bit 1 selects scanner 2.
void skannerztv_state::porta_out_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	m_porta_data = data;
}

// Delivers the second byte of a two-byte scanner response.
// We can't push both bytes in the tap callback because uart_rx_force's baud-rate
// continuation timer would delay byte 2 by ~17 ms — longer than WaitCycle (~6 ms).
// Instead we push byte 1 in the tap, then schedule this timer for a short delay so
// byte 2 arrives after byte 1's timer fires but well before WaitCycle expires.
TIMER_CALLBACK_MEMBER(skannerztv_state::byte2_tick)
{
	m_maincpu->uart_rx_force(m_byte2_pending);
	m_byte2_pending = 0;
}

// Monster/item type IDs for all 100 M/I slots.
// Values are ROM table indices into the monster attribute table at 0xe198 (8 words/entry).
// Each entry is a 16-bit value stored as HI<<8|LO in the scanner slot, then used by
// the game as pData.monsters[n] (slots 0-79) or pData.items[n-80] (slots 80-99).
// Entries were selected by scanning the ROM table for non-trivial (non-null) entries.
const uint16_t skannerztv_state::s_mi_cheat[100] = {
	// Slots 0-79: monster type IDs
	0x0000, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0009,
	0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f, 0x0010, 0x0012,
	0x0014, 0x0016, 0x0017, 0x0018, 0x0019, 0x001b, 0x001d, 0x001e,
	0x001f, 0x0020, 0x0021, 0x0022, 0x0024, 0x0026, 0x0027, 0x0028,
	0x0029, 0x002a, 0x002b, 0x002d, 0x002e, 0x002f, 0x0030, 0x0031,
	0x0032, 0x0033, 0x0034, 0x0036, 0x0037, 0x0038, 0x0039, 0x003a,
	0x003b, 0x003c, 0x003d, 0x003e, 0x003f, 0x0040, 0x0041, 0x0042,
	0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004a,
	0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0051, 0x0052, 0x0053,
	0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x005a, 0x005b, 0x005c,
	// Slots 80-99: item type IDs
	0x005d, 0x005e, 0x005f, 0x0060, 0x0061, 0x0062, 0x0063, 0x0064,
	0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c,
	0x006d, 0x006e, 0x006f, 0x0070,
};

// Write tap on UART TXBUF (word addr 0x3d35): fires synchronously when the firmware
// writes to TxBuf, before the baud timer starts.  This sidesteps the timing mismatch
// where WaitCycle (~6 ms) expires before a 600-baud byte (~17 ms) could arrive.
//
// Protocol (per Tahg's reverse engineering, see file header pseudocode):
//   0x01          → 0x02          (probe ACK)
//   n|0x80 n=0..99 → HI, LO      (M/I load: monster/item type IDs)
//   0x03          → BLO, BHI     (button poll)
//   0x04          → 0x05         (save request ACK)
//   n|0x80, HI, LO (save mode) → 0x06 per slot (save ACK, 100 slots total)
void skannerztv_state::uart_txbuf_tap(offs_t offset, uint16_t &data, uint16_t mem_mask)
{
	uint8_t tx = data & 0xff;
	logerror("uart_txbuf_tap: tx=%02x porta=%04x state=%d mi=%d save_byte=%d\n",
	         tx, m_porta_data, m_vsk_state, m_vsk_mi, m_save_byte);

	if (tx == 0x01)
	{
		// Only respond during probe phase. Once M/I loading starts we ignore all
		// further probes — the "losing" player keeps re-probing and responding
		// would inject spurious 0x02 bytes that corrupt the M/I data stream.
		//
		// Dedup: both doPlayer(1) and doPlayer(2) write TXBUF=0x01 at the same
		// machine timestamp.  One ACK per distinct timestamp → exactly one 0x02
		// in UARTBuffer for whichever player's state-1 check runs first.
		if (m_vsk_state == 0)
		{
			attotime now = machine().time();
			if (now != m_last_probe_time)
			{
				m_last_probe_time = now;
				m_vsk_state = 1;
				m_vsk_mi = 0;
				m_maincpu->uart_rx_force(0x02);
			}
		}
	}
	else if (tx == 0x04 && m_vsk_state == 2)
	{
		// Save request: console wants to write M/I data back to scanner.
		// ACK with 0x05, then expect 100 × (index|0x80, HI, LO) sequences.
		m_maincpu->uart_rx_force(0x05);
		m_vsk_state = 3;
		m_vsk_mi = 0;
		m_save_byte = 0;
	}
	else if (m_vsk_state == 3)
	{
		// Save mode: firmware sends 3 bytes per slot (index|0x80, HI, LO).
		// We ACK only the final LO byte; the index and HI bytes are discarded
		// since we have no persistent storage to update.
		if (m_save_byte < 2)
		{
			m_save_byte++;
		}
		else
		{
			m_maincpu->uart_rx_force(0x06);
			m_save_byte = 0;
			if (++m_vsk_mi >= 100)
				m_vsk_state = 2;
		}
	}
	else if (tx == 0x03)
	{
		// Button poll: BLO immediately, BHI (unused) via timer.
		int ch = (m_porta_data & 2) ? 1 : 0;
		optional_ioport &scan = (ch == 0) ? m_io_scan1 : m_io_scan2;
		uint8_t blo = scan.found() ? (scan->read() & 0x3f) : 0;
		m_maincpu->uart_rx_force(blo);
		m_byte2_pending = 0x00;
		m_byte2_timer->adjust(attotime::from_usec(500));
	}
	else if (tx >= 0x80 && tx <= 0xe3 && m_vsk_state == 1)
	{
		// M/I slot request: return the cheat table entry for this slot index.
		// HI byte immediately, LO byte via timer (both well within WaitCycle ~6 ms).
		uint16_t id = s_mi_cheat[m_vsk_mi];
		m_maincpu->uart_rx_force(uint8_t(id >> 8));
		m_byte2_pending = uint8_t(id & 0xff);
		m_byte2_timer->adjust(attotime::from_usec(500));
		if (++m_vsk_mi >= 100)
			m_vsk_state = 2;
	}
}

static INPUT_PORTS_START( rad_sktv )
	/* Console GPIO buttons (IOA port, active-LOW).
	   To access internal test mode: hold DOWN and BUTTON1 at startup until a
	   coloured screen appears; press BUTTON1 to cycle through tests. */

	PORT_START("P1")
	PORT_DIPNAME( 0x0001, 0x0001, "IN0" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("B")
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("A")
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )

	PORT_START("P2")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P3")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNUSED )

	/* Scanner UART buttons, delivered as BLO byte in response to command 0x03.
	   BLO bits: 0=Up, 1=Down, 2=B, 3=A, 4=Left, 5=Right (active-high, 1=pressed). */
	PORT_START("SCAN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )    PORT_PLAYER(1) PORT_NAME("Scanner 1 Up")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )  PORT_PLAYER(1) PORT_NAME("Scanner 1 Down")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON4 )        PORT_PLAYER(1) PORT_NAME("Scanner 1 B")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON3 )        PORT_PLAYER(1) PORT_NAME("Scanner 1 A")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )  PORT_PLAYER(1) PORT_NAME("Scanner 1 Left")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1) PORT_NAME("Scanner 1 Right")
	PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("SCAN2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )    PORT_PLAYER(2) PORT_NAME("Scanner 2 Up")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )  PORT_PLAYER(2) PORT_NAME("Scanner 2 Down")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON4 )        PORT_PLAYER(2) PORT_NAME("Scanner 2 B")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON3 )        PORT_PLAYER(2) PORT_NAME("Scanner 2 A")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )  PORT_PLAYER(2) PORT_NAME("Scanner 2 Left")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2) PORT_NAME("Scanner 2 Right")
	PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

void skannerztv_state::rad_sktv(machine_config &config)
{
	SPG24X(config, m_maincpu, XTAL(27'000'000), m_screen);
	m_maincpu->set_addrmap(AS_PROGRAM, &skannerztv_state::mem_map_4m);

	spg2xx_base(config);

	m_maincpu->porta_in().set(FUNC(skannerztv_state::base_porta_r));
	m_maincpu->porta_out().set(FUNC(skannerztv_state::porta_out_w));
	m_maincpu->portb_in().set(FUNC(skannerztv_state::base_portb_r));
	m_maincpu->portc_in().set(FUNC(skannerztv_state::base_portc_r));

	// Scanner UART responses are handled via a write tap on TXBUF (0x3d35)
	// installed in machine_start(), not through uart_tx callback.

	//m_maincpu->i2c_w().set(FUNC(skannerztv_state::i2c_w));
	//m_maincpu->i2c_r().set(FUNC(skannerztv_state::i2c_r));
}


ROM_START( rad_sktv )
	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "skannerztv.bin", 0x000000, 0x200000, CRC(e92278e3) SHA1(eb6bee5e661128d83784960dfff50379c36bfaeb) )

	/* The external scanner MCU is a Winbond from 2000: SA5641
	   the scanner plays sound effects when scanning, without being connected to the main unit, so a way to dump / emulate
	   this MCU is also needed for complete emulation

	   TODO: find details on MCU so that we know capacity etc. */
ROM_END

} // anonymous namespace


CONS( 2007, rad_sktv,  0,        0, rad_sktv, rad_sktv,   skannerztv_state, init_crc, "Radica", "Skannerz TV",                 MACHINE_IMPERFECT_SOUND | MACHINE_NOT_WORKING )
