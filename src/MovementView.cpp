#include "MovementView.h"

MoveData::MoveData()
{}

MovementView::MovementView():
    View(101)
{}

bool MovementView::add(DataPacketPtr data)
{
    /*std::vector<std::string> types = data->getStringList("type");
    for(std::vector<std::string>::iterator typeIt = types.begin(); typeIt != types.end(); typeIt++)
    {
        if((*typeIt).compare("movable")==0)
        {
            MoveDataPtr dat(new MoveData());
            dat->lastMove = 0;
            dat->movedLastFrame = false;
            std::string name = data->getName();
            MoveEnt ent(name, dat);
            MoveRes res =  moveTiming.insert(ent);
            if(res.first == moveTiming.end() || res.second == false)
            {
                Event_System::getSingleton().queueEvent(EventPtr(new MsgEvt("Movement View","Move Timer with the name " + name + " wasn't created")));
                return false;
            }
            break;
        }
    }*/
    return true;
}

void MovementView::addMoveEvent(Evt_Move const & event)
{
    reqMoves.push_back(event);
}

/**********************************
*go through the move queue, see if*
*any movements are for the same   *
*entity then add them together    *
***********************************/
void MovementView::preFrame()
{
    for(MoveList::iterator it = reqMoves.begin(); it != reqMoves.end(); it++)
    {
        Evt_Move move = (*it);
        Evt_MoveData *dat = move.getDataPtr<Evt_MoveData>();
        uint32_t id = dat->ID;
        MoveList::iterator it2 = it;
        for(it2++;it2 != reqMoves.end();it2++)
        {
            Evt_MoveData *datComp = (*it2).getDataPtr<Evt_MoveData>();
            if(datComp->ID == id)
            {
                move.getDataPtr<Evt_MoveData>()->x += datComp->x;
                move.getDataPtr<Evt_MoveData>()->y += datComp->y;
                reqMoves.erase(it2); 
            }
        }
        movesToBeExecuted.push_back(move);
    }
}

/************************************
*check if Data needs to be moved    *
*if so then move it, pretty easy    *
*************************************/
void MovementView::onFrame(DataPacketPtr data)
{
    /*MoveMap::iterator timingResult = moveTiming.find(data->getName());
    if(timingResult == moveTiming.end() || movesToBeExecuted.empty())
    {
        return;
    }*/
    for(MoveList::iterator it = movesToBeExecuted.begin(); it != movesToBeExecuted.end(); it++)
    {
        Evt_MoveData *dat = (*it).getDataPtr<Evt_MoveData>();
        if(data->getID() == dat->ID)
        {
            data->set("x", data->getInt("x")+dat->x);
            data->set("y", data->getInt("Y")+dat->y);
            continue;
        }
    }
}

/************************************
*Check the move queue, if there are *
*any events in the queue then print *
*an error message                   *
*************************************/
void MovementView::postFrame()
{
    reqMoves.clear();
    movesToBeExecuted.clear();
}

void MovementView::remove(std::string name)
{
    
}

bool MovementView::init()
{
    return true;
}
