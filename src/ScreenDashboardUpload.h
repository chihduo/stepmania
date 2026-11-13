#ifndef SCREEN_DASHBOARD_UPLOAD_H
#define SCREEN_DASHBOARD_UPLOAD_H
#include "ScreenWithMenuElements.h"
#include "DashboardSync.h"
#include "BitmapText.h"
#include "Quad.h"
class ScreenDashboardUpload : public ScreenWithMenuElements
{
public:
	void Init() override;
	void BeginScreen() override;
	void Update(float delta) override;
	bool InputShortcut(const InputEventPlus& input) override;
	bool Input(const InputEventPlus& input) override;
	void HandleScreenMessage(ScreenMessage message) override;
private:
	enum Action { Upload, Token, DashboardPage, BuildLog, Back, NUM_ACTIONS };
	void Refresh();
	void Activate();
	void Feedback(const RString& heading, const RString& details,
		Dashboard::StatusKind kind, bool includeJob = false);
	Dashboard::Job m_Job;
	Quad m_Backdrop;
	BitmapText m_Title, m_Status, m_Details, m_Actions[NUM_ACTIONS], m_Help;
	RString m_FeedbackHeading, m_FeedbackDetails;
	Dashboard::StatusKind m_FeedbackKind = Dashboard::StatusKind::Progress;
	int m_Selection=Upload, m_DisplayedSelection=-1, m_DisplayedKind=-1;
	float m_StartDelay=.25f, m_LayoutScale=1, m_ActionTop=0;
	bool m_Started=false, m_Leaving=false, m_IncludeJob=false;
};
#endif
