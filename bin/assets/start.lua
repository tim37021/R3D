fonts={"Delicious-Roman.otf", "Delicious-Italic.otf", "Delicious-Bold.otf", "Delicious-BoldItalic.otf"}

obj_sel=nil

function LoadFont()
	for k, v in pairs(fonts) do
		rocket:LoadFontFace("assets/"..v);
	end
end

function LoadFile(element)
	local t_x=tonumber(element:GetElementById("t_x").attributes.value)
	local t_y=tonumber(element:GetElementById("t_y").attributes.value)
	local t_z=tonumber(element:GetElementById("t_z").attributes.value)

	local r_x=tonumber(element:GetElementById("r_x").attributes.value)
	local r_y=tonumber(element:GetElementById("r_y").attributes.value)
	local r_z=tonumber(element:GetElementById("r_z").attributes.value)

	local zoom=tonumber(element:GetElementById("zoom").attributes.value)

	obj_sel=LoadObjScene(fn);
	obj_sel:SetRotation(r_x,r_y,r_z);
	obj_sel:SetTranslation(t_x,t_y,t_z);
	obj_sel:SetScale(zoom);
	propertymenu:GetElementById("xpos"):SetAttribute("value",obj_sel:GetTranslation("x"));
	propertymenu:GetElementById("ypos"):SetAttribute("value",obj_sel:GetTranslation("y"));
	propertymenu:GetElementById("zpos"):SetAttribute("value",obj_sel:GetTranslation("z"));
	propertymenu:GetElementById("xrot"):SetAttribute("value",obj_sel:GetRotation("x"));
	propertymenu:GetElementById("yrot"):SetAttribute("value",obj_sel:GetRotation("y"));
	propertymenu:GetElementById("zrot"):SetAttribute("value",obj_sel:GetRotation("z"));
	propertymenu:GetElementById("zoom"):SetAttribute("value",obj_sel:GetScale());
end

function OnFilesDropIn(filenames)
	loadingwindow:Show();
	fn=filenames[1]
end

function updatems(mpx,mpy)
	mspos:GetElementById("mousepos").inner_rml="mx: "..mpx.." my:"..mpy
end

function Startup()
	LoadFont()
	maincontext = rocket.contexts["main"]
	debugwindow = maincontext:LoadDocument("assets/debug.rml")
	debugwindow:GetElementById("title").inner_rml="Debug"
	debugwindow:Show()
	
	loadingwindow=maincontext:LoadDocument("assets/loading.rml")
	loadingwindow:GetElementById("title").inner_rml="Load"
	
	propertymenu=maincontext:LoadDocument("assets/promenu.rml")
	propertymenu:Show()
	comwin=maincontext:LoadDocument("assets/cmd.rml")
	comwin:GetElementById("title").inner_rml="LUA Command"
	comwin:GetElementById("window").style["display"]="none"
	comwin.style["height"]="0px";
	comwin:Show()
	scnlist=maincontext:LoadDocument("assets/demo.rml")
	scnlist:GetElementById("title").inner_rml="Scene List"
	scnlist:Show()
	
	maincontext:LoadDocument("assets/menu.rml"):Show()
	mspos=maincontext:LoadDocument("assets/mspos.rml")
	mspos:Show()
end

Startup()
