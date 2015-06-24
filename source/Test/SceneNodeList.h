#include <string>
#include <r3d/r3d.hpp>
#include <Rocket/Controls/DataSource.h>
#include <vector>
#include <cstdio>

class SceneNodeList:public Rocket::Controls::DataSource
{
	public:
		struct data{
			std::string name;
			std::string type;
			r3d::SceneNode *ptr;
		};
		
		r3d::Engine *engine;
		r3d::SceneNode *currentEntry;
		std::string current_table;

		SceneNodeList(const char *name, r3d::Engine *engine_):Rocket::Controls::DataSource(name), engine(engine_)
		{
			currentEntry=nullptr;
			current_table="/";
		}

		void GetRow(Rocket::Core::StringList& row, const Rocket::Core::String& table, int row_index, const Rocket::Core::StringList& columns)
		{
			current_table=table.CString();
			if(!currentEntry) return;
			auto lit=currentEntry->getChildren().cbegin();
			for(int i=0;i<row_index;i++,lit++);
			for (size_t i = 0; i < columns.size(); i++){
				if (columns[i] == "name")
				{
					row.push_back((*lit)->getName());
				}
				else if (columns[i] == "type")
				{
					row.push_back((*lit)->getNodeType());
				}
				else if (columns[i] == "ptr")
				{
					row.push_back(std::to_string((intptr_t)(*lit).get()).c_str());
				}
			}
		}
	

	int GetNumRows(const Rocket::Core::String& table)
	{
		auto sMgr=engine->getSceneManager();
		std::string entryName;
		currentEntry=nullptr;
		for(uint32_t i=0; i<table.Length(); i++)
		{
			if(table[i]!='/')
				entryName+=table[i];
			else
			{
				if(currentEntry)
				{
					// find the entry
					auto &children=currentEntry->getChildren();
					currentEntry=nullptr;
					for(auto &child: children)
					{
						if(entryName==child->getName())
						{
							currentEntry=child.get();
							break;
						}
					}
					// not found
					if(currentEntry==nullptr)
						return 0;
				}else
					currentEntry=sMgr->getRootNode().get();

				entryName="";
			}
		}

		if(currentEntry&&entryName!="")
		{
			// find the entry
			auto &children=currentEntry->getChildren();
			currentEntry=nullptr;
			for(auto &child: children)
			{
				if(entryName==child->getName())
				{
					currentEntry=child.get();
					break;
				}
			}
			// not found
			if(currentEntry==nullptr)
				return 0;
		}else
			currentEntry=sMgr->getRootNode().get();

		current_table=table.CString();

		return currentEntry?currentEntry->getChildren().size(): 0;
	}
	
	void notify(r3d::SceneNode *node)
	{
		std::string path;

		r3d::SceneNode *thisNode=node;

		while(node)
		{
			path = std::string("/")+node->getName() + path;
			node = node->getParent();
		}

		NotifyRowAdd(path.c_str(), thisNode->getChildren().size()-1, 1);
	}
};