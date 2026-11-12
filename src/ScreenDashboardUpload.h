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
	void Update(float delta) override;
	bool InputShortcut(const InputEventPlus& input) override;
	bool Input(const InputEventPlus& input) override;
	void HandleScreenMessage(ScreenMessage message) override;
private:
	void Refresh();
	void Activate();
	Dashboard::Job m_Job;
	Quad m_Backdrop;
	BitmapText m_Title,m_Status,m_Actions,m_Help;
	int m_Selection=0;
	float m_StartDelay=.25f;
	bool m_Started=false,m_Leaving=false;
};
#endif
