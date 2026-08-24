/// \file
/// \brief This file contains enumerations for packet priority and reliability enumerations.
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.rakkarsoft.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __PACKET_PRIORITY_H
#define __PACKET_PRIORITY_H 

/// These enumerations are used to describe when packets are delivered.
enum PacketPriority
{
	SYSTEM_PRIORITY,   /// 内部由 RakNet 用来发送高于高优先级的消息。
	HIGH_PRIORITY,   /// 高优先级消息在中优先级消息之前发送。
	MEDIUM_PRIORITY,   /// 中优先级消息在低优先级消息之前发送。
	LOW_PRIORITY,   /// 低优先级消息仅在没有其他消息等待时才发送。
	NUMBER_OF_PRIORITIES
};

/// These enumerations are used to describe how packets are delivered.
/// \note  Note to self: I write this with 3 bits in the stream.  If I add more remember to change that
enum PacketReliability
{
	UNRELIABLE = 6,   /// 与常规 UDP 相同，除了它还会丢弃重复的数据报。 RakNet 增加了（6 到 17）+21 位的开销，其中 16 位用于检测重复数据包，6 到 17 位用于消息长度。
	UNRELIABLE_SEQUENCED,  /// 带有序列计数器的常规 UDP。 乱序消息将被丢弃。 这在用于 UNRELIABLE 的顶部添加了额外的 13 位。
	RELIABLE,   /// 数据包将被可靠发送，但不一定按顺序发送。 与 UNRELIABLE 相同的开销。
	RELIABLE_ORDERED,   /// 可靠数据包，并且会按照您发送它的的顺序到达。 乱序消息将被延迟。 与 UNRELIABLE_SEQUENCED 相同的开销。
	RELIABLE_SEQUENCED /// 可靠数据包，并且会按照您发送它的顺序到达。 乱序消息将被丢弃。 与 UNRELIABLE_SEQUENCED 相同的开销。
};

#endif
