#ifndef DATAMODEL_H_INCLUDED
#define DATAMODEL_H_INCLUDED
#include <string>
#include <iostream>
#include "Process.h"
#include "Methods.h"
#include "MsgEvt.h"
#include "View.h"
#include "Event_System.h"
#include "Evt_CloseApplication.h"
#include "Evt_Context.h"
#include <boost/shared_ptr.hpp>
#include <map>
#include <json/json.h>
#include "DataPacket.h"

class DataModel : public Process
{
    public:
        typedef std::map<uint32_t, DataPacketPtr> DataMap;
        typedef std::pair<uint32_t, DataPacketPtr> DataEnt;
        typedef std::pair<DataMap::iterator, bool> DataRes;
        typedef boost::shared_ptr<const DataMap> DataConstPtr;
        typedef boost::shared_ptr<DataMap> DataMapPtr;

        DataModel(std::string name);
        ~DataModel();
        bool loadFile(std::string filename);
        bool loadMenu(std::string filename);
        bool showMenu();
        bool hideMenu();
        void tick();
        bool init(){return true;}
        bool addView(ViewPtr obj);
        void removeView(uint32_t id);
        bool addSilentView(ViewPtr obj);
        bool addObject(std::string name, std::string filename, std::string values);
        bool removeObject(std::string name);
        DataPacketPtr getObject(std::string name);
        DataPacketPtr getObject(uint32_t id);
        DataConstPtr getDataList();
        static Json::Value getRoot(std::string filename);
    private:
        typedef std::vector<ViewPtr> ViewList;

        DataMapPtr data;
        DataMapPtr menu;
        ViewList views, silentViews;
        std::string curContext;
        bool inMenu;
};

typedef boost::shared_ptr<DataModel> DataModelPtr;
#endif
