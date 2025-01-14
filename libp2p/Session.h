/*
	This file is part of cpp-ethereum.

	cpp-ethereum is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-ethereum is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file Session.h
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#pragma once

#include <mutex>
#include <array>
#include <deque>
#include <set>
#include <memory>
#include <utility>
#include <libdevcore/Common.h>
#include <libdevcore/RLP.h>
#include <libdevcore/RangeMask.h>
#include <libdevcore/Guards.h>
#include "Common.h"

namespace dev
{

namespace p2p
{

struct Node;

/**
 * @brief The Session class
 * @todo Document fully.
 */
class Session: public std::enable_shared_from_this<Session>
{
	friend class Host;
	friend class HostCapabilityFace;

public:
	Session(Host* _server, bi::tcp::socket _socket, std::shared_ptr<Node> const& _n, bool _force = false);
	Session(Host* _server, bi::tcp::socket _socket, bi::tcp::endpoint const& _manual);
	virtual ~Session();

	void start();
	void disconnect(DisconnectReason _reason);

	void ping();

	bool isOpen() const { return m_socket.is_open(); }

	NodeId id() const;
	unsigned socketId() const { return m_socket.native_handle(); }

	bi::tcp::endpoint endpoint() const;	///< for other peers to connect to.

	template <class PeerCap>
	std::shared_ptr<PeerCap> cap() const { try { return std::static_pointer_cast<PeerCap>(m_capabilities.at(std::make_pair(PeerCap::name(), PeerCap::version()))); } catch (...) { return nullptr; } }

	static RLPStream& prep(RLPStream& _s, PacketType _t, unsigned _args = 0);
	static RLPStream& prep(RLPStream& _s);
	void sealAndSend(RLPStream& _s);
	void send(bytes&& _msg);
	void send(bytesConstRef _msg);

	int rating() const;
	void addRating(unsigned _r);

	void addNote(std::string const& _k, std::string const& _v) { m_info.notes[_k] = _v; }

	PeerInfo const& info() const { return m_info; }

	void ensureNodesRequested();
	void serviceNodesRequest();

private:
	/// Drop the connection for the reason @a _r.
	void drop(DisconnectReason _r);

	/// Perform a read on the socket.
	void doRead();

	/// Perform a single round of the write operation. This could end up calling itself asynchronously.
	void write();

	/// Interpret an incoming message.
	bool interpret(RLP const& _r);

	/// @returns true iff the _msg forms a valid message for sending or receiving on the network.
	static bool checkPacket(bytesConstRef _msg);

	Host* m_server;							///< The host that owns us. Never null.

	mutable bi::tcp::socket m_socket;		///< Socket for the peer's connection. Mutable to ask for native_handle().
	Mutex x_writeQueue;						///< Mutex for the write queue.
	std::deque<bytes> m_writeQueue;			///< The write queue.
	std::array<byte, 65536> m_data;			///< Data buffer for the write queue.
	bytes m_incoming;						///< The incoming read queue of bytes.

	PeerInfo m_info;						///< Dyanamic information about this peer.

	unsigned m_protocolVersion = 0;			///< The protocol version of the peer.
	std::shared_ptr<Node> m_node;			///< The Node object. Might be null if we constructed using a bare address/port.
	bi::tcp::endpoint m_manualEndpoint;		///< The endpoint as specified by the constructor.
	bool m_force = false;					///< If true, ignore IDs being different. This could open you up to MitM attacks.
	bool m_dropped = false;					///< If true, we've already divested ourselves of this peer. We're just waiting for the reads & writes to fail before the shared_ptr goes OOS and the destructor kicks in.

	bool m_theyRequestedNodes = false;		///< Has the peer requested nodes from us without receiveing an answer from us?
	bool m_weRequestedNodes = false;		///< Have we requested nodes from the peer and not received an answer yet?

	std::chrono::steady_clock::time_point m_connect;		///< Time point of connection.
	std::chrono::steady_clock::time_point m_ping;			///< Time point of last ping.
	std::chrono::steady_clock::time_point m_lastReceived;	///< Time point of last message.

	std::map<CapDesc, std::shared_ptr<Capability>> m_capabilities;	///< The peer's capability set.
	RangeMask<unsigned> m_knownNodes;		///< Nodes we already know about as indices into Host's nodesList. These shouldn't be resent to peer.
};

}
}
