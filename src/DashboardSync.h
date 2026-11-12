#ifndef DASHBOARD_SYNC_H
#define DASHBOARD_SYNC_H

#include "json/json.h"
#include <atomic>
#include <functional>
#include <map>
#include <mutex>
#include <thread>

class XNode;
namespace Dashboard
{
struct Settings
{
	bool enabled = false;
	RString repository, branch, profile, credential;
};
struct Snapshot
{
	Settings settings;
	RString guid, name, stats;
	bool machine = false;
	Json::Value songs;
	vector<RString> logs;
	// Remote path -> local immutable export file.
	std::map<RString, RString> files;
};
struct Response { int code = 0; RString body; };
using Request = std::function<Response(const RString&, const RString&, const RString&)>;
using Progress = std::function<void(const RString&)>;
Settings GetSettings();
RString ProfileChoices();
bool SaveCredential(const RString& token, RString& error);
RString Hash(const RString& data);
RString Base64(const RString& data);
RString Unbase64(const RString& data);
RString Sanitize(const XNode& source);
Snapshot Capture(); // Main thread only: freezes all live profile/song data.
void Export(Snapshot& snapshot, const Progress& progress, const std::atomic<bool>& cancel);
RString Publish(const Snapshot& snapshot, const Request& request, const Progress& progress,
	const std::atomic<bool>& cancel); // Testable GitHub protocol, no platform dependencies.

class Job
{
public:
	~Job();
	void Start();
	void Cancel() { m_Cancel = true; }
	bool Busy() const { return m_Busy; }
	RString Status() const;
private:
	void SetStatus(const RString& text);
	void Run(Snapshot snapshot);
	std::thread m_Thread;
	std::atomic<bool> m_Busy{false}, m_Cancel{false};
	mutable std::mutex m_Mutex;
	RString m_Status;
};
}
#endif
