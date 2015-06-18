function ktevent(event,type)
	if obj_sel~=nil then
		element=event.current_element
		if event.parameters['key_identifier']==72 then				
			if type=='x' then
				setTrans(element.parent_node,element:GetAttribute('value')-obj_sel:GetTranslation('x'),0,0)
			end
			if type=='y' then
				setTrans(element.parent_node,0,element:GetAttribute('value')-obj_sel:GetTranslation('y'),0)
			end
			if type=='z' then
				setTrans(element.parent_node,0,0,element:GetAttribute('value')-obj_sel:GetTranslation('z'))
			end
		end
	end
end

function setTrans(element,x,y,z)
	if obj_sel~=nil then
		x=obj_sel:GetTranslation('x')+x
		y=obj_sel:GetTranslation('y')+y
		z=obj_sel:GetTranslation('z')+z
		obj_sel:SetTranslation(x,y,z)
		element:GetElementById("xpos"):SetAttribute("value",x)
		element:GetElementById("ypos"):SetAttribute("value",y)
		element:GetElementById("zpos"):SetAttribute("value",z)
	end
end
			
function krevent(event,type)
			if obj_sel~=nil then
				element=event.current_element
				if event.parameters['key_identifier']==72 then				
				if type=='x' then
					setRot(element.parent_node,element:GetAttribute('value')-obj_sel:GetRotation('x'),0,0)
				end
				if type=='y' then
					setRot(element.parent_node,0,element:GetAttribute('value')-obj_sel:GetRotation('y'),0)
				end
				if type=='z' then
					setRot(element.parent_node,0,0,element:GetAttribute('value')-obj_sel:GetRotation('z'))
				end
				end
			end
end

function setRot(element,x,y,z)
			if obj_sel~=nil then
				x=obj_sel:GetRotation('x')+x
				y=obj_sel:GetRotation('y')+y
				z=obj_sel:GetRotation('z')+z
				obj_sel:SetRotation(x,y,z)
				element:GetElementById("xrot"):SetAttribute("value",x)
				element:GetElementById("yrot"):SetAttribute("value",y)
				element:GetElementById("zrot"):SetAttribute("value",z)
			end
end
			
function kzevent(event)
			if obj_sel~=nil then
				element=event.current_element
				if event.parameters['key_identifier']==72 then				
					setzoom(element.parent_node,element:GetAttribute('value')/obj_sel:GetScale('x'))
					
				end
			end
end

function setzoom(element,x)
			if obj_sel~=nil then
				x=obj_sel:GetScale('x')*x
				obj_sel:SetScale(x,x,x)
				element:GetElementById("zoom"):SetAttribute("value",x)
			end
end