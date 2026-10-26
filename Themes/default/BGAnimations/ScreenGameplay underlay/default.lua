local t = Def.ActorFrame {};
	t[#t+1] = LoadActor("ScreenFilter");
	t[#t+1] = LoadActor(THEME:GetPathG("LifeHistory", "graph"), true);
return t