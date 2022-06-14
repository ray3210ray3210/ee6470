///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2019 Cadence Design Systems, Inc. All rights reserved worldwide.
//
// The code contained herein is the proprietary and confidential information
// of Cadence or its licensors, and is supplied subject to a previously
// executed license and maintenance agreement between Cadence and customer.
// This code is intended for use with Cadence high-level synthesis tools and
// may not be used with other high-level synthesis tools. Permission is only
// granted to distribute the code as indicated. Cadence grants permission for
// customer to distribute a copy of this code to any partner to aid in designing
// or verifying the customer's intellectual property, as long as such
// distribution includes a restriction of no additional distributions from the
// partner, unless the partner receives permission directly from Cadence.
//
// ALL CODE FURNISHED BY CADENCE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, AND CADENCE SPECIFICALLY DISCLAIMS ANY WARRANTY OF NONINFRINGEMENT,
// FITNESS FOR A PARTICULAR PURPOSE OR MERCHANTABILITY. CADENCE SHALL NOT BE
// LIABLE FOR ANY COSTS OF PROCUREMENT OF SUBSTITUTES, LOSS OF PROFITS,
// INTERRUPTION OF BUSINESS, OR FOR ANY OTHER SPECIAL, CONSEQUENTIAL OR
// INCIDENTAL DAMAGES, HOWEVER CAUSED, WHETHER FOR BREACH OF WARRANTY,
// CONTRACT, TORT, NEGLIGENCE, STRICT LIABILITY OR OTHERWISE.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DEFINES_H_
#define _DEFINES_H_

typedef uint32_t input_t;
typedef uint32_t output_t;
typedef uint32_t data_t;
#define DATA_NUM 64     //Number of stimulus values per file
                        // must be even 
#define DATA_NUM_L1 16
#define DATA_NUM_L2 32
#define DATA_NUM_L3 64
#define FILE_NUM 8
#define SORT_START_POSITION 0

unsigned int fourCharToInt(unsigned char a, unsigned char b, unsigned char c, unsigned char d){
	unsigned int ret_val = 0;
	ret_val  =  a;
	ret_val <<= 8;
	ret_val |=  b;
	ret_val <<= 8;
	ret_val |=  c;
	ret_val <<= 8;
	ret_val |=  d;

	return ret_val;
}
#endif // _DEFINES_H_
