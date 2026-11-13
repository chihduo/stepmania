#include "global.h"
#include "ScreenDashboardUpload.h"
#include "ActorUtil.h"
#include "InputEventPlus.h"
#include "ScreenManager.h"
#include "ScreenDimensions.h"
#include "ScreenTextEntry.h"
#include "ThemeManager.h"
#include "RageUtil.h"
#include "arch/ArchHooks/ArchHooks.h"
#include <exception>
REGISTER_SCREEN_CLASS(ScreenDashboardUpload);
AutoScreenMessage(SM_DashboardToken);
void ScreenDashboardUpload::Init()
{
	ScreenWithMenuElements::Init();
	m_Backdrop.SetName("DashboardBackdrop");
	m_Backdrop.StretchTo(RectF(SCREEN_LEFT,SCREEN_TOP,SCREEN_RIGHT,SCREEN_BOTTOM));
	m_Backdrop.SetDiffuse(RageColor(.04f,.06f,.10f,1)); m_Backdrop.SetDrawOrder(999); AddChild(&m_Backdrop);
	m_Title.SetName("DashboardTitle"); m_Status.SetName("DashboardStatus");
	m_Actions.SetName("DashboardActions"); m_Help.SetName("DashboardHelp");
	for(auto* text:{&m_Title,&m_Status,&m_Actions,&m_Help})
	{
		text->LoadFromFont(THEME->GetPathF("Common","Normal")); text->SetDrawOrder(1000);
		text->SetShadowLength(1); text->SetMaxWidth(SCREEN_WIDTH-60); AddChild(text);
	}
	m_Title.SetXY(SCREEN_CENTER_X,SCREEN_TOP+50); m_Title.SetZoom(.9f); m_Title.SetText("UPLOAD STATISTICS");
	m_Status.SetXY(SCREEN_CENTER_X,SCREEN_TOP+155); m_Status.SetZoom(.55f);
	m_Status.SetMaxWidth((SCREEN_WIDTH-60)/.55f); m_Status.SetMaxHeight(150/.55f);
	m_Status.SetWrapWidthPixels(int((SCREEN_WIDTH-80)/.55f));
	m_Actions.SetXY(SCREEN_CENTER_X,SCREEN_BOTTOM-135); m_Actions.SetZoom(.65f);
	m_Help.SetXY(SCREEN_CENTER_X,SCREEN_BOTTOM-38); m_Help.SetZoom(.42f); m_Help.SetMaxWidth((SCREEN_WIDTH-40)/.42f);
	m_Help.SetText("Up / Down: select   Start: choose   Back: return\nConnection settings: Save/Preferences.ini   Setup: Docs/DashboardUpload.md");
	Actor* actors[]={&m_Backdrop,&m_Title,&m_Status,&m_Actions,&m_Help};
	for(Actor* actor:actors) ActorUtil::LoadAllCommands(*actor,m_sName);
	SortByDrawOrder(); Refresh();
}
void ScreenDashboardUpload::Refresh()
{
	RString status=m_Job.Status();
	if(status.empty()) status="Preparing upload...";
	if(!m_Feedback.empty()) status=m_Feedback+"\n\n"+status;
	m_Status.SetText(status);
	RString text; const char* choices[]={"Upload / Retry","Set GitHub Token","Open Dashboard","Open Build Log","Back"};
	for(int i=0;i<5;++i) text+=(i==m_Selection?">  ":"    ")+RString(choices[i])+"\n";
	m_Actions.SetText(text);
}
void ScreenDashboardUpload::Update(float delta)
{
	ScreenWithMenuElements::Update(delta);
	if(!m_Started && (m_StartDelay-=delta)<=0) { m_Started=true; m_Job.Start(); }
	if(m_Leaving && !m_Job.Busy()) { SCREENMAN->SetNewScreen("ScreenTitleMenu"); return; }
	Refresh();
}
void ScreenDashboardUpload::Activate()
{
	if(m_Selection==4)
	{
		m_Leaving=true; m_Job.Cancel();
		if(!m_Job.Busy()) SCREENMAN->SetNewScreen("ScreenTitleMenu");
		return;
	}
	if(m_Selection==0)
	{
		m_Feedback.clear();
		if(m_Job.Busy()) m_Feedback="Retry skipped because an upload was already running. Current status is shown below.";
		else if(!m_Job.Start()) m_Feedback="Upload could not start. Check the connection settings below.";
		SCREENMAN->SystemMessage(m_Feedback.empty()?"Preparing statistics...":"Upload did not start. See the status message.");
		return;
	}
	if(m_Selection==1)
	{
		if(m_Job.Busy())
		{
			m_Feedback="Token entry skipped because an upload was already running. Try again after it finishes.";
			return;
		}
		ScreenTextEntry::Password(SM_DashboardToken,"GitHub token (dashboard repository, Contents: read/write)"); return;
	}
	try
	{
		RString url=Dashboard::WebURL(Dashboard::GetSettings(),m_Selection==3);
		if(HOOKS->GoToURL(url)) m_Feedback="Opened in your browser:\n"+url;
		else m_Feedback="Could not open your browser. Open this address manually:\n"+url;
	}
	catch(const std::exception& e) { m_Feedback=RString("Cannot open this page.\n")+e.what(); }
}
bool ScreenDashboardUpload::InputShortcut(const InputEventPlus& input) { Input(input); return true; }
bool ScreenDashboardUpload::Input(const InputEventPlus& input)
{
	if(input.type!=IET_FIRST_PRESS) return true;
	bool key=input.DeviceI.device==DEVICE_KEYBOARD;
	if((key && input.DeviceI.button==KEY_ESC) || input.MenuI==GAME_BUTTON_BACK) { m_Selection=4; Activate(); }
	else if((key && input.DeviceI.button==KEY_UP) || input.MenuI==GAME_BUTTON_UP) m_Selection=(m_Selection+4)%5;
	else if((key && input.DeviceI.button==KEY_DOWN) || input.MenuI==GAME_BUTTON_DOWN) m_Selection=(m_Selection+1)%5;
	else if((key && input.DeviceI.button==KEY_ENTER) || input.MenuI==GAME_BUTTON_START) Activate();
	Refresh(); return true;
}
void ScreenDashboardUpload::HandleScreenMessage(ScreenMessage message)
{
	ScreenWithMenuElements::HandleScreenMessage(message);
	if(message==SM_DashboardToken)
	{
		if(!ScreenTextEntry::s_bCancelledLast)
		{
			RString error;
			bool ok=Dashboard::SaveCredential(ScreenTextEntry::s_sLastAnswer,error);
			std::fill(ScreenTextEntry::s_sLastAnswer.begin(),ScreenTextEntry::s_sLastAnswer.end(),'\0');
			ScreenTextEntry::s_sLastAnswer.clear();
			m_Feedback=ok?"GitHub token saved in Windows Credential Manager. Select Upload / Retry.":"Could not save the GitHub token.\n"+error;
			if(ok) m_Selection=0;
			SCREENMAN->SystemMessage(ok?"GitHub token saved. Select Upload / Retry.":"Could not save the GitHub token. See the status message.");
		}
		else m_Feedback="Token entry cancelled. The saved token was not changed.";
		Refresh();
	}
}
