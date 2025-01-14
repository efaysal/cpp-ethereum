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
/** @file QEthereum.h
 * @authors:
 *   Gav Wood <i@gavwood.com>
 *   Marek Kotewicz <marek@ethdev.com>
 * @date 2014
 */

#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <jsonrpc/rpc.h>

class QWebThree: public QObject
{
	Q_OBJECT
	
public:
	QWebThree(QObject* _p);
	virtual ~QWebThree();

	void poll();
	void clearWatches();
	void clientDieing();
	
	Q_INVOKABLE void postMessage(QString _json);
	
public slots:
	void onDataProcessed(QString _json, QString _addInfo);
	
signals:
	void processData(QString _json, QString _addInfo);
	void response(QString _json);
	void onNewId(QString _id);
	
private:
	std::vector<unsigned> m_watches;
	std::vector<unsigned> m_shhWatches;
};

class QWebThreeConnector: public QObject, public jsonrpc::AbstractServerConnector
{
	Q_OBJECT
	
public:
	QWebThreeConnector();
	virtual ~QWebThreeConnector();
	
	void setQWeb(QWebThree *_q);
	
	virtual bool StartListening();
	virtual bool StopListening();
	virtual bool SendResponse(std::string const& _response, void* _addInfo = NULL);
	
public slots:
	void onProcessData(QString const& _json, QString const& _addInfo);

signals:
	void dataProcessed(QString const& _json, QString const& _addInfo);
	
private:
	QWebThree* m_qweb = nullptr;
	bool m_isListening;
};

#define QETH_INSTALL_JS_NAMESPACE(_frame, _env, qweb) [_frame, _env, qweb]() \
{ \
	_frame->disconnect(); \
	_frame->addToJavaScriptWindowObject("_web3", qweb, QWebFrame::ScriptOwnership); \
	_frame->evaluateJavaScript(contentsOfQResource(":/js/es6-promise-2.0.0.js")); \
	_frame->evaluateJavaScript(contentsOfQResource(":/js/main.js")); \
	_frame->evaluateJavaScript(contentsOfQResource(":/js/qt.js")); \
	_frame->evaluateJavaScript(contentsOfQResource(":/js/setup.js")); \
}


