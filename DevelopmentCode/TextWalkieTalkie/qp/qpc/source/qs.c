/**
* @file
* @brief QS software tracing services
* @ingroup qs
* @cond
******************************************************************************
* Last updated for version 5.4.0
* Last updated on  2015-04-23
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, www.state-machine.com.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* Web:   www.state-machine.com
* Email: info@state-machine.com
******************************************************************************
* @endcond
*/
#include "qs_port.h"      /* QS port */
#include "qs_pkg.h"       /* QS package-scope interface */
#include "qassert.h"      /* QP embedded systems-friendly assertions */

Q_DEFINE_THIS_MODULE("qs")

/****************************************************************************/
QSPriv QS_priv_;  /* QS private data */

/****************************************************************************/
/**
* @description
* This function should be called from QS_onStartup() to provide QS with
* the data buffer. The first parameter @p sto[] is the address of the memory
* block, and the second parameter @p stoSize is the size of this block
* in bytes. Currently the size of the QS buffer cannot exceed 64KB.
*
* @note QS can work with quite small data buffers, but you will start losing
* data if the buffer is too small for the bursts of tracing activity.
* The right size of the buffer depends on the data production rate and
* the data output rate. QS offers flexible filtering to reduce the data
* production rate.
*
* @note If the data output rate cannot keep up with the production rate,
* QS will start overwriting the older data with newer data. This is
* consistent with the "last-is-best" QS policy. The record sequence counters
* and check sums on each record allow the QSPY host uitiliy to easily detect
* any data loss.
*/
void QS_initBuf(uint8_t sto[], uint_fast16_t stoSize) {
    uint8_t *buf = &sto[0];

    /* must be at least 8 bytes */
    Q_REQUIRE_ID(100, stoSize > (uint_fast16_t)8);

    /* The QS_initBuf() function clears the internal QS variables, so that
    * the tracing can start correctly even if the startup code fails
    * to clear the uninitialized data (as is required by the C Standard).
    */
    QF_bzero(&QS_priv_, (uint_fast16_t)sizeof(QS_priv_));
    QS_priv_.buf = buf;
    QS_priv_.end = (QSCtr)stoSize;

    QS_beginRec((uint_fast8_t)QS_EMPTY);
    QS_endRec();
    QS_beginRec((uint_fast8_t)QS_QP_RESET);
    QS_endRec();
}

/****************************************************************************/
/**
* @description
* This function sets up the QS filter to enable the record type @p rec.
* The argument @a #QS_ALL_RECORDS specifies to filter-in all records.
* This function should be called indirectly through the macro QS_FILTER_ON.
*
* @note Filtering based on the record-type is only the first layer of
* filtering. The second layer is based on the object-type. Both filter
* layers must be enabled for the QS record to be inserted into the QS buffer.
* @sa QS_filterOff(), QS_FILTER_SM_OBJ, QS_FILTER_AO_OBJ, QS_FILTER_MP_OBJ,
* QS_FILTER_EQ_OBJ, and QS_FILTER_TE_OBJ.
*/
void QS_filterOn(uint_fast8_t rec) {
    if (rec == QS_ALL_RECORDS) {
        uint_fast8_t i;
        for (i = (uint_fast8_t)0;
             i < (uint_fast8_t)(sizeof(QS_priv_.glbFilter) - 1U);
             ++i)
        {
            QS_priv_.glbFilter[i] = (uint8_t)0xFFU; /* set all bits */
        }
        /* never turn the last 3 records on (0x7D, 0x7E, 0x7F) */
        QS_priv_.glbFilter[sizeof(QS_priv_.glbFilter) - 1U] = (uint8_t)0x1F;
    }
    else {
        /* record numbers can't exceed QS_ESC, so they don't need escaping */
        Q_ASSERT_ID(210, rec < (uint_fast8_t)QS_ESC);
        QS_priv_.glbFilter[rec >> 3] |=
            (uint8_t)(1U << (rec & (uint_fast8_t)7));
    }
}

/****************************************************************************/
/**
* @description
* This function sets up the QS filter to disable the record type @p rec.
* The argument @a #QS_ALL_RECORDS specifies to suppress all records.
* This function should be called indirectly through the macro QS_FILTER_OFF.
*
* @note Filtering records based on the record-type is only the first layer of
* filtering. The second layer is based on the object-type. Both filter
* layers must be enabled for the QS record to be inserted into the QS buffer.
*/
void QS_filterOff(uint_fast8_t rec) {
    if (rec == QS_ALL_RECORDS) {
        uint8_t *glbFilter = &QS_priv_.glbFilter[0];

        /* the following unrolled loop is designed to stop collecting trace
        * very fast in order to prevent overwriting the interesting data.
        * The code assumes that the dimension of QS_priv_.glbFilter[] is 16.
        */
        *glbFilter = (uint8_t)0;  QS_PTR_INC_(glbFilter);
        *glbFilter = (uint8_t)0;  QS_PTR_INC_(glbFilter);
        *glbFilter = (uint8_t)0;  QS_PTR_INC_(glbFilter);
        *glbFilter = (uint8_t)0;  QS_PTR_INC_(glbFilter);

        *glbFilter = (uint8_t)0;  QS_PTR_INC_(glbFilter);
        *glbFilter = (uint8_t)0;  QS_PTR_INC_(glbFilter);
        *glbFilter = (uint8_t)0;  QS_PTR_INC_(glbFilter);
        *glbFilter = (uint8_t)0;  QS_PTR_INC_(glbFilter);

        *glbFilter = (uint8_t)0;  QS_PTR_INC_(glbFilter);
        *glbFilter = (uint8_t)0;  QS_PTR_INC_(glbFilter);
        *glbFilter = (uint8_t)0;  QS_PTR_INC_(glbFilter);
        *glbFilter = (uint8_t)0;  QS_PTR_INC_(glbFilter);

        *glbFilter = (uint8_t)0;  QS_PTR_INC_(glbFilter);
        *glbFilter = (uint8_t)0;  QS_PTR_INC_(glbFilter);
        *glbFilter = (uint8_t)0;  QS_PTR_INC_(glbFilter);
        *glbFilter = (uint8_t)0;
    }
    else {
        uint8_t tmp;
        /* record numbers can't exceed QS_ESC, so they don't need escaping */
        Q_ASSERT_ID(310, rec < (uint_fast8_t)QS_ESC);
        tmp =  (uint8_t)(1U << (rec & (uint_fast8_t)0x07));
        tmp ^= (uint8_t)0xFFU; /* invert all bits */
        QS_priv_.glbFilter[rec >> 3] &= tmp;
    }
}

/****************************************************************************/
/**
* @description
* This function must be called at the beginning of each QS record.
* This function should be called indirectly through the macro #QS_BEGIN,
* or #QS_BEGIN_NOCRIT, depending if it's called in a normal code or from
* a critical section.
*/
void QS_beginRec(uint_fast8_t rec) {
    uint8_t b      = (uint8_t)(QS_priv_.seq + (uint8_t)1);
    uint8_t chksum = (uint8_t)0;      /* reset the checksum */
    uint8_t *buf   = QS_priv_.buf;    /* put in a temporary (register) */
    QSCtr   head   = QS_priv_.head;   /* put in a temporary (register) */
    QSCtr   end    = QS_priv_.end;    /* put in a temporary (register) */

    QS_priv_.seq = b; /* store the incremented sequence num */
    QS_priv_.used += (QSCtr)2; /* 2 bytes about to be added */

    QS_INSERT_ESC_BYTE(b)

    chksum = (uint8_t)(chksum + (uint8_t)rec); /* update checksum */
    QS_INSERT_BYTE((uint8_t)rec) /* rec byte does not need escaping */

    QS_priv_.head   = head;   /* save the head */
    QS_priv_.chksum = chksum; /* save the checksum */
}

/****************************************************************************/
/**
* @description
* This function must be called at the end of each QS record.
* This function should be called indirectly through the macro #QS_END,
* or #QS_END_NOCRIT, depending if it's called in a normal code or from
* a critical section.
*/
void QS_endRec(void) {
    uint8_t *buf = QS_priv_.buf;  /* put in a temporary (register) */
    QSCtr   head = QS_priv_.head;
    QSCtr   end  = QS_priv_.end;
    uint8_t b = QS_priv_.chksum;
    b ^= (uint8_t)0xFFU;   /* invert the bits in the checksum */

    QS_priv_.used += (QSCtr)2; /* 2 bytes about to be added */

    if ((b != QS_FRAME) && (b != QS_ESC)) {
        QS_INSERT_BYTE(b)
    }
    else {
        QS_INSERT_BYTE(QS_ESC)
        QS_INSERT_BYTE(b ^ QS_ESC_XOR)
        ++QS_priv_.used; /* account for the ESC byte */
    }

    QS_INSERT_BYTE(QS_FRAME) /* do not escape this QS_FRAME */

    QS_priv_.head = head; /* save the head */

    /* overrun over the old data? */
    if (QS_priv_.used > end) {
        QS_priv_.used = end;   /* the whole buffer is used */
        QS_priv_.tail = head;  /* shift the tail to the old data */
    }
}

/****************************************************************************/
/**
* @description
* @note This function is only to be used through macros, never in the
* client code directly.
*/
void QS_u8(uint8_t format, uint8_t d) {
    uint8_t chksum = QS_priv_.chksum; /* put in a temporary (register) */
    uint8_t *buf   = QS_priv_.buf;    /* put in a temporary (register) */
    QSCtr   head   = QS_priv_.head;   /* put in a temporary (register) */
    QSCtr   end    = QS_priv_.end;    /* put in a temporary (register) */

    QS_priv_.used += (QSCtr)2; /* 2 bytes about to be added */

    QS_INSERT_ESC_BYTE(format)
    QS_INSERT_ESC_BYTE(d)

    QS_priv_.head   = head;   /* save the head */
    QS_priv_.chksum = chksum; /* save the checksum */
}

/****************************************************************************/
/**
* @description
* This function is only to be used through macros, never in the
* client code directly.
*/
void QS_u16(uint8_t format, uint16_t d) {
    uint8_t chksum = QS_priv_.chksum; /* put in a temporary (register) */
    uint8_t *buf   = QS_priv_.buf;    /* put in a temporary (register) */
    QSCtr   head   = QS_priv_.head;   /* put in a temporary (register) */
    QSCtr   end    = QS_priv_.end;    /* put in a temporary (register) */

    QS_priv_.used += (QSCtr)3; /* 3 bytes about to be added */

    QS_INSERT_ESC_BYTE(format)

    format = (uint8_t)d;
    QS_INSERT_ESC_BYTE(format)

    d >>= 8;
    format = (uint8_t)d;
    QS_INSERT_ESC_BYTE(format)

    QS_priv_.head   = head;   /* save the head */
    QS_priv_.chksum = chksum; /* save the checksum */
}

/****************************************************************************/
/**
* @note This function is only to be used through macros, never in the
* client code directly.
*/
void QS_u32(uint8_t format, uint32_t d) {
    uint8_t chksum = QS_priv_.chksum; /* put in a temporary (register) */
    uint8_t *buf   = QS_priv_.buf;    /* put in a temporary (register) */
    QSCtr   head   = QS_priv_.head;   /* put in a temporary (register) */
    QSCtr   end    = QS_priv_.end;    /* put in a temporary (register) */
    int_fast8_t   i;

    QS_priv_.used += (QSCtr)5; /* 5 bytes about to be added */
    QS_INSERT_ESC_BYTE(format) /* insert the format byte */

    /* insert 4 bytes... */
    for (i = (int_fast8_t)4; i != (int_fast8_t)0; --i) {
        format = (uint8_t)d;
        QS_INSERT_ESC_BYTE(format)
        d >>= 8;
    }

    QS_priv_.head   = head;   /* save the head */
    QS_priv_.chksum = chksum; /* save the checksum */
}

/****************************************************************************/
/*! output uint8_t data element without format information */
/** @note This function is only to be used through macros, never in the
* client code directly.
*/
void QS_u8_(uint8_t d) {
    uint8_t chksum = QS_priv_.chksum; /* put in a temporary (register) */
    uint8_t *buf = QS_priv_.buf;      /* put in a temporary (register) */
    QSCtr   head = QS_priv_.head;     /* put in a temporary (register) */
    QSCtr   end  = QS_priv_.end;      /* put in a temporary (register) */

    ++QS_priv_.used; /* 1 byte about to be added */
    QS_INSERT_ESC_BYTE(d)

    QS_priv_.head   = head;    /* save the head */
    QS_priv_.chksum = chksum;  /* save the checksum */
}

/****************************************************************************/
/** @note This function is only to be used through macros, never in the
* client code directly.
*/
void QS_u8u8_(uint8_t d1, uint8_t d2) {
    uint8_t chksum = QS_priv_.chksum; /* put in a temporary (register) */
    uint8_t *buf = QS_priv_.buf;      /* put in a temporary (register) */
    QSCtr   head = QS_priv_.head;     /* put in a temporary (register) */
    QSCtr   end  = QS_priv_.end;      /* put in a temporary (register) */

    QS_priv_.used += (QSCtr)2; /* 2 bytes are about to be added */
    QS_INSERT_ESC_BYTE(d1)
    QS_INSERT_ESC_BYTE(d2)

    QS_priv_.head   = head;    /* save the head */
    QS_priv_.chksum = chksum;  /* save the checksum */
}

/****************************************************************************/
/**
* @note This function is only to be used through macros, never in the
* client code directly.
*/
void QS_u16_(uint16_t d) {
    uint8_t b      = (uint8_t)d;
    uint8_t chksum = QS_priv_.chksum; /* put in a temporary (register) */
    uint8_t *buf = QS_priv_.buf;      /* put in a temporary (register) */
    QSCtr   head = QS_priv_.head;     /* put in a temporary (register) */
    QSCtr   end  = QS_priv_.end;      /* put in a temporary (register) */

    QS_priv_.used += (QSCtr)2; /* 2 bytes are about to be added */

    QS_INSERT_ESC_BYTE(b)

    d >>= 8;
    b = (uint8_t)d;
    QS_INSERT_ESC_BYTE(b)

    QS_priv_.head   = head;    /* save the head */
    QS_priv_.chksum = chksum;  /* save the checksum */
}

/****************************************************************************/
/** @note This function is only to be used through macros, never in the
* client code directly.
*/
void QS_u32_(uint32_t d) {
    uint8_t chksum = QS_priv_.chksum; /* put in a temporary (register) */
    uint8_t *buf = QS_priv_.buf;      /* put in a temporary (register) */
    QSCtr   head = QS_priv_.head;     /* put in a temporary (register) */
    QSCtr   end  = QS_priv_.end;      /* put in a temporary (register) */
    int_fast8_t i;

    QS_priv_.used += (QSCtr)4; /* 4 bytes are about to be added */
    for (i = (int_fast8_t)4; i != (int_fast8_t)0; --i) {
        uint8_t b = (uint8_t)d;
        QS_INSERT_ESC_BYTE(b)
        d >>= 8;
    }

    QS_priv_.head   = head;    /* save the head */
    QS_priv_.chksum = chksum;  /* save the checksum */
}

/****************************************************************************/
/**
* @note This function is only to be used through macros, never in the
* client code directly.
*/
void QS_str_(char_t const *s) {
    uint8_t b      = (uint8_t)(*s);
    uint8_t chksum = QS_priv_.chksum; /* put in a temporary (register) */
    uint8_t *buf = QS_priv_.buf;      /* put in a temporary (register) */
    QSCtr   head = QS_priv_.head;     /* put in a temporary (register) */
    QSCtr   end  = QS_priv_.end;      /* put in a temporary (register) */
    QSCtr   used = QS_priv_.used;     /* put in a temporary (register) */

    while (b != (uint8_t)(0)) {
        chksum = (uint8_t)(chksum + b); /* update checksum */
        QS_INSERT_BYTE(b)  /* ASCII characters don't need escaping */
        QS_PTR_INC_(s);
        b = (uint8_t)(*s);
        ++used;
    }
    QS_INSERT_BYTE((uint8_t)0)  /* zero-terminate the string */
    ++used;

    QS_priv_.head   = head;   /* save the head */
    QS_priv_.chksum = chksum; /* save the checksum */
    QS_priv_.used   = used;   /* save # of used buffer space */
}

/****************************************************************************/
/**
* @note This function is only to be used through macros, never in the
* client code directly.
*/
void QS_str_ROM_(char_t const Q_ROM *s) {
    uint8_t b      = (uint8_t)Q_ROM_BYTE(*s);
    uint8_t chksum = QS_priv_.chksum; /* put in a temporary (register) */
    uint8_t *buf = QS_priv_.buf;      /* put in a temporary (register) */
    QSCtr   head = QS_priv_.head;     /* put in a temporary (register) */
    QSCtr   end  = QS_priv_.end;      /* put in a temporary (register) */
    QSCtr   used = QS_priv_.used;     /* put in a temporary (register) */

    while (b != (uint8_t)(0)) {
        chksum = (uint8_t)(chksum + b); /* update checksum */
        QS_INSERT_BYTE(b)  /* ASCII characters don't need escaping */
        QS_PTR_INC_(s);
        b = (uint8_t)Q_ROM_BYTE(*s);
        ++used;
    }
    QS_INSERT_BYTE((uint8_t)0) /* zero-terminate the string */
    ++used;

    QS_priv_.head   = head;   /* save the head */
    QS_priv_.chksum = chksum; /* save the checksum */
    QS_priv_.used   = used;   /* save # of used buffer space */
}

/****************************************************************************/
/**
* @description
* This function delivers one byte at a time from the QS data buffer.
*
* @returns the byte in the least-significant 8-bits of the 16-bit return
* value if the byte is available. If no more data is available at the time,
* the function returns ::QS_EOD (End-Of-Data).
*
* @note QS_getByte() is __not__ protected with a critical section.
*/
uint16_t QS_getByte(void) {
    uint16_t ret;
    if (QS_priv_.used == (QSCtr)0) {
        ret = QS_EOD; /* set End-Of-Data */
    }
    else {
        uint8_t *buf = QS_priv_.buf;  /* put in a temporary (register) */
        QSCtr tail   = QS_priv_.tail; /* put in a temporary (register) */
        ret = (uint16_t)(*QS_PTR_AT_(tail)); /* set the byte to return */
        ++tail; /* advance the tail */
        if (tail == QS_priv_.end) { /* tail wrap around? */
            tail = (QSCtr)0;
        }
        QS_priv_.tail = tail; /* update the tail */
        --QS_priv_.used;      /* one less byte used */
    }
    return ret; /* return the byte or EOD */
}

/****************************************************************************/
/**
* @description
* This function delivers a contiguous block of data from the QS data buffer.
* The function returns the pointer to the beginning of the block, and writes
* the number of bytes in the block to the location pointed to by @p pNbytes.
* The parameter @p pNbytes is also used as input to provide the maximum size
* of the data block that the caller can accept.
*
* @returns if data is available, the function returns pointer to the
* contiguous block of data and sets the value pointed to by @p pNbytes
* to the # available bytes. If data is available at the time the function is
* called, the function returns NULL pointer and sets the value pointed to by
* @p pNbytes to zero.
*
* @note Only the NULL return from QS_getBlock() indicates that the QS buffer
* is empty at the time of the call. The non-NULL return often means that
* the block is at the end of the buffer and you need to call QS_getBlock()
* again to obtain the rest of the data that "wrapped around" to the
* beginning of the QS data buffer.
*
* @note QS_getBlock() is NOT protected with a critical section.
*/
uint8_t const *QS_getBlock(uint16_t *pNbytes) {
    QSCtr used = QS_priv_.used; /* put in a temporary (register) */
    uint8_t *buf;

    /* any bytes used in the ring buffer? */
    if (used != (QSCtr)0) {
        QSCtr tail = QS_priv_.tail;  /* put in a temporary (register) */
        QSCtr end  = QS_priv_.end;   /* put in a temporary (register) */
        QSCtr n = (QSCtr)(end - tail);
        if (n > used) {
            n = used;
        }
        if (n > (QSCtr)(*pNbytes)) {
            n = (QSCtr)(*pNbytes);
        }
        *pNbytes = (uint16_t)n;      /* n-bytes available */
        buf = QS_priv_.buf;
        buf = QS_PTR_AT_(tail);      /* the bytes are at the tail */

        QS_priv_.used = (QSCtr)(used - n);
        tail += n;
        if (tail == end) {
            tail = (QSCtr)0;
        }
        QS_priv_.tail = tail;
    }

    else { /* no bytes available */
        *pNbytes = (uint16_t)0;  /* no bytes available right now */
        buf      = (uint8_t *)0; /* no bytes available right now */
    }
    return buf;
}

/****************************************************************************/
/** @note This function is only to be used through macro QS_SIG_DICTIONARY()
*/
void QS_sig_dict(enum_t const sig, void const * const obj,
                 char_t const Q_ROM * const name)
{
    QS_CRIT_STAT_
    QS_CRIT_ENTRY_();
    QS_beginRec((uint_fast8_t)QS_SIG_DICT);
    QS_SIG_((QSignal)sig);
    QS_OBJ_(obj);
    QS_STR_ROM_(name);
    QS_endRec();
    QS_CRIT_EXIT_();
    QS_onFlush();
}

/****************************************************************************/
/** @note This function is only to be used through macro QS_OBJ_DICTIONARY()
*/
void QS_obj_dict(void const * const obj,
                 char_t const Q_ROM * const name)
{
    QS_CRIT_STAT_
    QS_CRIT_ENTRY_();
    QS_beginRec((uint_fast8_t)QS_OBJ_DICT);
    QS_OBJ_(obj);
    QS_STR_ROM_(name);
    QS_endRec();
    QS_CRIT_EXIT_();
    QS_onFlush();
}

/****************************************************************************/
/** @note This function is only to be used through macro QS_FUN_DICTIONARY()
*/
void QS_fun_dict(void (* const fun)(void),
                 char_t const Q_ROM * const name)
{
    QS_CRIT_STAT_
    QS_CRIT_ENTRY_();
    QS_beginRec((uint_fast8_t)QS_FUN_DICT);
    QS_FUN_(fun);
    QS_STR_ROM_(name);
    QS_endRec();
    QS_CRIT_EXIT_();
    QS_onFlush();
}

/****************************************************************************/
/** @note This function is only to be used through macro QS_USR_DICTIONARY()
*/
void QS_usr_dict(enum_t const rec,
                 char_t const Q_ROM * const name)
{
    QS_CRIT_STAT_
    QS_CRIT_ENTRY_();
    QS_beginRec((uint_fast8_t)QS_USR_DICT);
    QS_U8_((uint8_t)rec);
    QS_STR_ROM_(name);
    QS_endRec();
    QS_CRIT_EXIT_();
    QS_onFlush();
}

/****************************************************************************/
/** @note This function is only to be used through macros, never in the
* client code directly.
*/
void QS_mem(uint8_t const *blk, uint8_t size) {
    uint8_t b      = (uint8_t)(QS_MEM_T);
    uint8_t chksum = (uint8_t)(QS_priv_.chksum + b);
    uint8_t *buf   = QS_priv_.buf;  /* put in a temporary (register) */
    QSCtr   head   = QS_priv_.head; /* put in a temporary (register) */
    QSCtr   end    = QS_priv_.end;  /* put in a temporary (register) */

    QS_priv_.used += ((QSCtr)size + (QSCtr)2); /* size+2 bytes to be added */

    QS_INSERT_BYTE(b)
    QS_INSERT_ESC_BYTE(size)

    /* output the 'size' number of bytes */
    while (size != (uint8_t)0) {
        b = *blk;
        QS_INSERT_ESC_BYTE(b)
        QS_PTR_INC_(blk);
        --size;
    }

    QS_priv_.head   = head;   /* save the head */
    QS_priv_.chksum = chksum; /* save the checksum */
}

/****************************************************************************/
/**
* @note This function is only to be used through macros, never in the
* client code directly.
*/
void QS_str(char_t const *s) {
    uint8_t b      = (uint8_t)(*s);
    uint8_t chksum = (uint8_t)(QS_priv_.chksum + (uint8_t)QS_STR_T);
    uint8_t *buf   = QS_priv_.buf;  /* put in a temporary (register) */
    QSCtr   head   = QS_priv_.head; /* put in a temporary (register) */
    QSCtr   end    = QS_priv_.end;  /* put in a temporary (register) */
    QSCtr   used   = QS_priv_.used; /* put in a temporary (register) */

    used += (QSCtr)2; /* account for the format byte and the terminating-0 */
    QS_INSERT_BYTE((uint8_t)QS_STR_T)
    while (b != (uint8_t)(0)) {
        /* ASCII characters don't need escaping */
        chksum = (uint8_t)(chksum + b); /* update checksum */
        QS_INSERT_BYTE(b)
        QS_PTR_INC_(s);
        b = (uint8_t)(*s);
        ++used;
    }
    QS_INSERT_BYTE((uint8_t)0) /* zero-terminate the string */

    QS_priv_.head   = head;    /* save the head */
    QS_priv_.chksum = chksum;  /* save the checksum */
    QS_priv_.used   = used;    /* save # of used buffer space */
}

/****************************************************************************/
/** @note This function is only to be used through macros, never in the
* client code directly.
*/
void QS_str_ROM(char_t const Q_ROM *s) {
    uint8_t b      = (uint8_t)Q_ROM_BYTE(*s);
    uint8_t chksum = (uint8_t)(QS_priv_.chksum + (uint8_t)QS_STR_T);
    uint8_t *buf   = QS_priv_.buf;  /* put in a temporary (register) */
    QSCtr   head   = QS_priv_.head; /* put in a temporary (register) */
    QSCtr   end    = QS_priv_.end;  /* put in a temporary (register) */
    QSCtr   used   = QS_priv_.used; /* put in a temporary (register) */

    used += (QSCtr)2; /* account for the format byte and the terminating-0 */

    QS_INSERT_BYTE((uint8_t)QS_STR_T)
    while (b != (uint8_t)(0)) {
        /* ASCII characters don't need escaping */
        chksum = (uint8_t)(chksum + b); /* update checksum */
        QS_INSERT_BYTE(b)
        QS_PTR_INC_(s);
        b = (uint8_t)Q_ROM_BYTE(*s);
        ++used;
    }
    QS_INSERT_BYTE((uint8_t)0) /* zero-terminate the string */

    QS_priv_.head   = head;    /* save the head */
    QS_priv_.chksum = chksum;  /* save the checksum */
    QS_priv_.used   = used;    /* save # of used buffer space */
}
