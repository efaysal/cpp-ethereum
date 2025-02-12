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
/** @file trie.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 * Trie test functions.
 */

#include <fstream>
#include <random>
#include "JsonSpiritHeaders.h"
#include <libdevcore/CommonIO.h>
#include <libethereum/BlockChain.h>
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace dev;
using namespace dev::eth;

namespace js = json_spirit;

BOOST_AUTO_TEST_CASE(genesis_tests)
{
	const char* ptestPath = getenv("ETHEREUM_TEST_PATH");
	string testPath;

	if (ptestPath == NULL)
	{
		cnote << " could not find environment variable ETHEREUM_TEST_PATH \n";
		testPath = "../../../tests";
	}
	else
		testPath = ptestPath;

	cnote << "Testing Genesis block...";
	js::mValue v;
	string s = asString(contents(testPath + "/genesishashestest.json"));
	BOOST_REQUIRE_MESSAGE(s.length() > 0, "Contents of 'genesishashestest.json' is empty. Have you cloned the 'tests' repo branch develop?");
	js::read_string(s, v);

	js::mObject o = v.get_obj();

	BOOST_CHECK_EQUAL(BlockChain::genesis().stateRoot, h256(o["genesis_state_root"].get_str()));
	BOOST_CHECK_EQUAL(toHex(BlockChain::createGenesisBlock()), toHex(fromHex(o["genesis_rlp_hex"].get_str())));
	BOOST_CHECK_EQUAL(sha3(BlockChain::createGenesisBlock()), h256(o["genesis_hash"].get_str()));
}

