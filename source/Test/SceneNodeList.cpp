#include<string>
#include<Rocket/Controls/DataSource.h>
#include<r3d/r3d.hpp>
#include<vector>

class SceneNodeList:public Rocket::Controls::DataSource
{
	public :
		struct data{
			std::string name;
			std::string type;
			r3d::SceneNode *ptr;
		};
		std::vector<data> list;
		SceneNodeList(char *name):Rocket::Controls::DataSource(name){
			
		}

		void GetRow(Rocket::Core::StringList& row, const Rocket::Core::String& table, int row_index, const Rocket::Core::StringList& columns)
		{
		if (table == "list"){
			for (size_t i = 0; i < columns.size(); i++){
				if (columns[i] == "name"){
					row.push_back(list[row_index].name.c_str());
				}
				else if (columns[i] == "type"){
					row.push_back(list[row_index].type.c_str());
				}
			}
		}
	}

	int GetNumRows(const Rocket::Core::String& table)
	{
		if (table == "list")
		{
			return list.size();
		}
		return 0;
	}
	
	void addnode(std::string na,std::string ty,r3d::SceneNode *pt)
	{
		data temp;
		temp.name=na;
		temp.type=ty;
		temp.ptr=pt;
		list.push_back(temp);
		NotifyRowAdd("list", list.size()-1, 1);
	}
};

static SceneNodeList scene("scene");