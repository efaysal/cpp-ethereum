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
/** @file QEthereum.cpp
 * @authors:
 *   Marek Kotewicz <marek@ethdev.com>
 * @date 2014
 */

navigator.qt = _web3;

(function () {
	navigator.qt.handlers = [];
	Object.defineProperty(navigator.qt, 'onmessage', {
		set: function (handler) {
			navigator.qt.handlers.push(handler);
		}
	});
})();

navigator.qt.response.connect(function (res) {
	navigator.qt.handlers.forEach(function (handler) {
		handler({data: res});
	});
});

if (window.Promise === undefined) {
	window.Promise = ES6Promise.Promise;
}

web3.setProvider(new web3.providers.QtProvider());
