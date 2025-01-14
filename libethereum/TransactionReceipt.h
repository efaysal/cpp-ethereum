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
/** @file TransactionReceipt.h
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#pragma once

#include <array>
#include <map>
#include <libdevcore/Common.h>
#include <libdevcore/RLP.h>
#include <libevm/ExtVMFace.h>
#include "Manifest.h"

namespace dev
{

namespace eth
{

class TransactionReceipt
{
public:
	TransactionReceipt(bytesConstRef _rlp) { RLP r(_rlp); m_stateRoot = (h256)r[0]; m_gasUsed = (u256)r[1]; m_bloom = (LogBloom)r[2]; for (auto const& i: r[3]) m_log.emplace_back(i); }
	TransactionReceipt(h256 _root, u256 _gasUsed, LogEntries const& _log, Manifest const& _ms): m_stateRoot(_root), m_gasUsed(_gasUsed), m_bloom(eth::bloom(_log)), m_log(_log), m_changes(_ms) {}

	Manifest const& changes() const { return m_changes; }

	h256 const& stateRoot() const { return m_stateRoot; }
	u256 const& gasUsed() const { return m_gasUsed; }
	LogBloom const& bloom() const { return m_bloom; }
	LogEntries const& log() const { return m_log; }

	void streamRLP(RLPStream& _s) const
	{
		_s.appendList(4) << m_stateRoot << m_gasUsed << m_bloom;
		_s.appendList(m_log.size());
		for (LogEntry const& l: m_log)
			l.streamRLP(_s);
	}

private:
	h256 m_stateRoot;
	u256 m_gasUsed;
	LogBloom m_bloom;
	LogEntries m_log;

	Manifest m_changes;	///< TODO: PoC-7: KILL
};

using TransactionReceipts = std::vector<TransactionReceipt>;

}
}
