fonts={"Delicious-Roman.otf", "Delicious-Italic.otf", "Delicious-Bold.otf", "Delicious-BoldItalic.otf"}

obj_sel=nil -- the obj you operate now
obj_num=0 --total number of obj(exclude child node)
function LoadFont()
	for k, v in pairs(fonts) do
		rocket:LoadFontFace("assets/"..v);
	end
end

function LoadFile()
	obj_sel=LoadObjScene(fn);
	obj_num=obj_num+1
	tranmenu:GetElementById("xpos"):SetAttribute("value",obj_sel:GetTranslation("x"));
	tranmenu:GetElementById("ypos"):SetAttribute("value",obj_sel:GetTranslation("y"));
	tranmenu:GetElementById("zpos"):SetAttribute("value",obj_sel:GetTranslation("z"));
	tranmenu:GetElementById("xrot"):SetAttribute("value",obj_sel:GetRotation("x"));
	tranmenu:GetElementById("yrot"):SetAttribute("value",obj_sel:GetRotation("y"));
	tranmenu:GetElementById("zrot"):SetAttribute("value",obj_sel:GetRotation("z"));
	tranmenu:GetElementById("zoom"):SetAttribute("value",obj_sel:GetScale());
end

function OnFilesDropIn(filenames)
	fn=filenames[1]
	LoadFile()
end

function updatems(mpx,mpy)
	mspos:GetElementById("mousepos").inner_rml="mx: "..mpx.." my:"..mpy
end

function Startup()
	LoadFont()
	maincontext = rocket.contexts["main"]
	debugwindow = maincontext:LoadDocument("assets/debug.rml")
	debugwindow:GetElementById("title").inner_rml="Debug"
	debugwindow:GetElementById("window").style["display"]="none"
	debugwindow.style["height"]="0px";
	debugwindow:Show()
	
	loadingwindow=maincontext:LoadDocument("assets/loading.rml")
	loadingwindow:GetElementById("title").inner_rml="Load"
	
	tranmenu=maincontext:LoadDocument("assets/promenu.rml")
	tranmenu:GetElementById("title").inner_rml="Transformation"
	tranmenu:Show()
	comwin=maincontext:LoadDocument("assets/cmd.rml")
	comwin:GetElementById("title").inner_rml="LUA Command"
	comwin:GetElementById("window").style["display"]="none"
	comwin.style["height"]="0px";
	comwin:Show()
	scnlist=maincontext:LoadDocument("assets/demo.rml")
	scnlist:GetElementById("title").inner_rml="Scene List"
	local ds=scnlist:GetElementById("dt")
	sceneurl=scnlist:GetElementById("sceneurl")--!!!!!!!!!!!!!!!!!!!!!!!!!!!!URL
	ElementDataGrid.SetDataSource(ds,"scene."..scnlist:GetElementById("sceneurl"):GetAttribute("value"))
	scnlist:Show()
	
	maincontext:LoadDocument("assets/menu.rml"):Show()
	mspos=maincontext:LoadDocument("assets/mspos.rml")
	mspos:Show()
end

Startup()
