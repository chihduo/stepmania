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
using Dashboard::StatusKind;
void ScreenDashboardUpload::Init()
{
	ScreenWithMenuElements::Init();
	m_LayoutScale=SCREEN_HEIGHT/720.f;
	m_ActionTop=SCREEN_BOTTOM-300*m_LayoutScale;
	m_Backdrop.SetName("DashboardBackdrop");
	m_Backdrop.StretchTo(RectF(SCREEN_LEFT,SCREEN_TOP,SCREEN_RIGHT,SCREEN_BOTTOM));
	m_Backdrop.SetDiffuse(RageColor(.04f,.06f,.10f,1)); m_Backdrop.SetDrawOrder(999); AddChild(&m_Backdrop);
	m_Title.SetName("DashboardTitle"); m_Status.SetName("DashboardStatus");
	m_Details.SetName("DashboardDetails"); m_Help.SetName("DashboardHelp");
	for(auto* text:{&m_Title,&m_Status,&m_Details,&m_Help})
	{
		text->LoadFromFont(THEME->GetPathF("Common","Normal")); text->SetDrawOrder(1000);
		text->SetShadowLength(1); AddChild(text);
	}
	m_Title.SetXY(SCREEN_CENTER_X,SCREEN_TOP+50*m_LayoutScale);
	m_Title.SetZoom(1.25f*m_LayoutScale); m_Title.SetText("UPLOAD STATISTICS");
	m_Status.LoadFromFont(THEME->GetPathF(m_sName,"status"));
	m_Status.SetXY(SCREEN_CENTER_X,SCREEN_TOP+110*m_LayoutScale);
	m_Status.SetVertAlign(VertAlign_Top); m_Status.SetZoom(.9f*m_LayoutScale);
	m_Status.SetShadowLength(2); m_Status.SetMaxWidth((SCREEN_WIDTH-80*m_LayoutScale)/(.9f*m_LayoutScale));
	m_Status.SetMaxHeight(105/.9f);
	m_Status.SetWrapWidthPixels(int((SCREEN_WIDTH-80*m_LayoutScale)/(.9f*m_LayoutScale)));
	m_Details.SetX(SCREEN_CENTER_X); m_Details.SetVertAlign(VertAlign_Top);
	m_Details.SetZoom(.9f*m_LayoutScale); m_Details.SetShadowLength(0);
	m_Details.SetMaxWidth((SCREEN_WIDTH-80*m_LayoutScale)/(.9f*m_LayoutScale));
	m_Details.SetWrapWidthPixels(int((SCREEN_WIDTH-80*m_LayoutScale)/(.9f*m_LayoutScale)));
	for(int i=0;i<NUM_ACTIONS;++i)
	{
		auto& action=m_Actions[i];
		action.SetName(ssprintf("DashboardAction%d",i+1));
		action.LoadFromFont(THEME->GetPathF(m_sName,"actions"));
		action.SetXY(SCREEN_CENTER_X,m_ActionTop+46*i*m_LayoutScale);
		action.SetZoom(.75f*m_LayoutScale); action.SetShadowLength(1); action.SetDrawOrder(1000);
		action.SetMaxWidth((SCREEN_WIDTH-80*m_LayoutScale)/m_LayoutScale);
		AddChild(&action);
		ActorUtil::LoadAllCommandsFromName(action,m_sName,"DashboardAction");
	}
	m_Help.SetXY(SCREEN_CENTER_X,SCREEN_BOTTOM-38*m_LayoutScale); m_Help.SetZoom(.65f*m_LayoutScale);
	m_Help.SetMaxWidth((SCREEN_WIDTH-40*m_LayoutScale)/(.65f*m_LayoutScale));
	m_Help.SetText("Up / Down: select   Start: choose   Back: return\nConnection settings: Save/Preferences.ini   Setup: Docs/DashboardUpload.md");
	Actor* actors[]={&m_Backdrop,&m_Title,&m_Status,&m_Details,&m_Help};
	for(Actor* actor:actors) ActorUtil::LoadAllCommands(*actor,m_sName);
	SortByDrawOrder(); Refresh();
}
void ScreenDashboardUpload::BeginScreen()
{
	ScreenWithMenuElements::BeginScreen();
	m_DisplayedSelection=m_DisplayedKind=-1;
	Refresh();
}
void ScreenDashboardUpload::Feedback(const RString& heading, const RString& details, StatusKind kind, bool includeJob)
{
	m_FeedbackHeading=heading; m_FeedbackDetails=details;
	m_FeedbackKind=kind; m_IncludeJob=includeJob;
}
void ScreenDashboardUpload::Refresh()
{
	auto report=m_Job.GetStatus();
	RString heading=m_FeedbackHeading, details=m_FeedbackDetails;
	StatusKind kind=m_FeedbackKind;
	if(heading.empty())
	{
		kind=report.kind;
		if(kind==StatusKind::Error) { heading="Upload needs attention"; details=report.text; }
		else
		{
			size_t line=report.text.find('\n');
			heading=report.text.substr(0,line);
			if(line!=RString::npos) details=report.text.substr(line+1);
			if(heading.empty()) heading="Preparing upload...";
		}
	}
	else if(m_IncludeJob && !report.text.empty()) details+="\n\nUpload status: "+report.text;
	m_Status.SetText(heading); m_Details.SetText(details);
	float detailY=m_Status.GetY()+m_Status.GetZoomedHeight()+18*m_LayoutScale;
	m_Details.SetY(detailY);
	m_Details.SetMaxHeight(std::max(1.f,(m_ActionTop-40*m_LayoutScale-detailY)/(.9f*m_LayoutScale)));
	if(m_DisplayedKind!=int(kind))
	{
		const char* commands[]={"Progress","Success","Warning","Error"};
		const RageColor colors[]={RageColor(.35f,.78f,1,1),RageColor(.4f,.9f,.65f,1),RageColor(1,.76f,.3f,1),RageColor(1,.4f,.45f,1)};
		m_Status.SetDiffuse(colors[int(kind)]); m_Status.PlayCommand(commands[int(kind)]);
		m_DisplayedKind=int(kind);
	}
	if(m_DisplayedSelection!=m_Selection)
	{
		const char* choices[]={"Upload / Retry","Set GitHub Token","Open Dashboard","Open Build Log","Back"};
		for(int i=0;i<NUM_ACTIONS;++i)
		{
			bool selected=i==m_Selection;
			auto& action=m_Actions[i];
			action.SetText((selected?">  ":"")+RString(choices[i]));
			action.StopTweening(); action.BeginTweening(.1f);
			action.SetZoom((selected?1.f:.75f)*m_LayoutScale);
			action.SetDiffuse(selected?RageColor(.45f,.85f,1,1):RageColor(1,1,1,1));
			action.PlayCommand(selected?"GainFocus":"LoseFocus");
		}
		m_DisplayedSelection=m_Selection;
	}
}
void ScreenDashboardUpload::Update(float delta)
{
	ScreenWithMenuElements::Update(delta);
	if(!m_Started && !m_Leaving && SCREENMAN->GetTopScreen()==this && (m_StartDelay-=delta)<=0)
	{ m_Started=true; m_Job.Start(); }
	if(m_Leaving && !m_Job.Busy()) { SCREENMAN->SetNewScreen("ScreenTitleMenu"); return; }
	Refresh();
}
void ScreenDashboardUpload::Activate()
{
	if(m_Leaving)
	{
		Feedback("Please wait before choosing another option", "Returning to the title menu after the current request finishes.", StatusKind::Warning);
		return;
	}
	switch(m_Selection)
	{
	case Back:
		m_Started=true; m_Leaving=true; m_Job.Cancel();
		Feedback("Returning to the title menu...", "Stopping local work. An upload already accepted by GitHub may continue online.", StatusKind::Progress);
		if(!m_Job.Busy()) SCREENMAN->SetNewScreen("ScreenTitleMenu");
		return;
	case Upload:
		m_Started=true;
		if(m_Job.Busy())
			Feedback("Retry was not started", "An upload was already running. Wait for it to finish, or choose Back to stop local work.", StatusKind::Warning, true);
		else
		{
			Feedback("","",StatusKind::Progress);
			if(!m_Job.Start()) Feedback("Upload could not start",m_Job.Status(),StatusKind::Error);
		}
		return;
	case Token:
		if(m_Job.Busy())
		{
			Feedback("Token entry is unavailable", "An upload was already running. Change the token after it finishes, or choose Back to stop local work.", StatusKind::Warning, true);
			return;
		}
		ScreenTextEntry::Password(SM_DashboardToken,"GitHub token (dashboard repository, Contents: read/write)");
		return;
	case DashboardPage:
	case BuildLog:
		try
		{
			RString url=Dashboard::WebURL(Dashboard::GetSettings(),m_Selection==BuildLog);
			if(HOOKS->GoToURL(url)) Feedback(m_Selection==BuildLog?"Build log opened":"Dashboard opened", "Switch to your browser to view:\n"+url,StatusKind::Success,m_Job.Busy());
			else Feedback("Browser could not open", "Open this address manually:\n"+url,StatusKind::Warning,m_Job.Busy());
		}
		catch(const std::exception& e) { Feedback("Cannot open this page",e.what(),StatusKind::Error,m_Job.Busy()); }
		return;
	default:
		Feedback("Unavailable option", "Choose one of the listed menu options with Up / Down, then press Start.",StatusKind::Warning);
		m_Selection=Upload;
		return;
	}
}
bool ScreenDashboardUpload::InputShortcut(const InputEventPlus& input) { Input(input); return true; }
bool ScreenDashboardUpload::Input(const InputEventPlus& input)
{
	if(input.type!=IET_FIRST_PRESS) return true;
	bool key=input.DeviceI.device==DEVICE_KEYBOARD;
	if((key && input.DeviceI.button==KEY_ESC) || input.MenuI==GAME_BUTTON_BACK) { m_Selection=Back; Activate(); }
	else if((key && input.DeviceI.button==KEY_UP) || input.MenuI==GAME_BUTTON_UP) m_Selection=(m_Selection+NUM_ACTIONS-1)%NUM_ACTIONS;
	else if((key && input.DeviceI.button==KEY_DOWN) || input.MenuI==GAME_BUTTON_DOWN) m_Selection=(m_Selection+1)%NUM_ACTIONS;
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
			if(ok)
			{
				m_Selection=Upload;
				Feedback("GitHub token saved", "Select Upload / Retry to start. Connection settings are read from Save/Preferences.ini.",StatusKind::Success,true);
			}
			else Feedback("Could not save the GitHub token",error,StatusKind::Error);
		}
		else Feedback("Token entry cancelled", "Any saved token remains unchanged.",StatusKind::Warning);
		Refresh();
	}
}
