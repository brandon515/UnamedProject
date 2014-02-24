#include "SDLView.h"


SDLView::SDLView():
    View(0),
    window(NULL),
    renderer(NULL)
{
}

SDLView::~SDLView()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

bool SDLView::init()
{
    //SDL and Screen initalization

    if(SDL_Init(SDL_INIT_EVERYTHING)!=0)
    {
        std::string error(SDL_GetError());
        Event_System::getSingleton().queueEvent(EventPtr(new MsgEvt("SDL failed to initilize: " + error)));
        return false;
    }
    IMG_Init(IMG_INIT_PNG);
    Json::Value root = getRoot("WindowSettings");
    uint32_t flags = 0;
    if(root["fullscreen"].asBool() == true)
    {
        flags = SDL_WINDOW_FULLSCREEN_DESKTOP|SDL_WINDOW_BORDERLESS;
    }
    int w = 800, h = 600;
    if(root["width"].isInt())
    {
        w = root["width"].asInt();
    }
    if(root["height"].isInt())
    {
        h = root["height"].asInt();
    }
    std::string title = "New Window";
    if(root["title"].isString())
    {
        title = root["title"].asString();
    }
    window = SDL_CreateWindow(
                title.c_str(),
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                w,
                h,
                flags);
    if(window == NULL)
    {
        std::string error(SDL_GetError());
        Event_System::getSingleton().queueEvent(EventPtr(new MsgEvt("SDL window failed to initilize: " + error)));
        return false;
    }
    setID(SDL_GetWindowID(window));
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL)
    {
        std::string error(SDL_GetError());
        Event_System::getSingleton().queueEvent(EventPtr(new MsgEvt("SDL window failed to initilize: " + error)));
        return false;
    } 
    return true;
}

bool SDLView::add(DataPacketPtr data)
{
    std::string imageStr = data->getString("image");
    SDL_Texture *tex = NULL;
    SDL_Surface *sur = IMG_Load(imageStr.c_str());
    if(sur == NULL)
    {
            Event_System::getSingleton().queueEvent(EventPtr(new MsgEvt("SDL can't load image named " + imageStr + " \n\treason: " + IMG_GetError())));
            return false;
    }
    tex = SDL_CreateTextureFromSurface(renderer, sur);
    if(tex == NULL)
    {
            Event_System::getSingleton().queueEvent(EventPtr(new MsgEvt("SDL can't create texture from file named " + imageStr + " \n\treason: " + IMG_GetError())));
            return false;
    }
    SDL_FreeSurface(sur);
    uint32_t hash = CRC32(data->getName().c_str(), data->getName().length());
    TextureEnt ent(hash, tex);
    TextureRes res = textures.insert(ent);
    if(res.first == textures.end() || res.second == false)
    {
            Event_System::getSingleton().queueEvent(EventPtr(new MsgEvt("Texture " + imageStr + " couldn't be added into the Map")));
            return false;
    }
    return true;
}

void SDLView::remove(std::string name)
{
    TextureMap::iterator it = textures.find(CRC32(name.c_str(), name.length()));
    textures.erase(it);
}

void SDLView::preFrame()
{
    SDL_RenderClear(renderer);
}

void SDLView::onFrame(DataPacketPtr data)
{
    uint32_t hash = CRC32(data->getName().c_str(), data->getName().length());
    TextureMap::iterator it = textures.find(hash);
    SDL_Texture *temp = it->second;
    SDL_Rect rect;
    rect.x = data->getInt("x");
    rect.y = data->getInt("y");
    SDL_QueryTexture(temp, NULL, NULL, &rect.w, &rect.h);
    SDL_RenderCopy(renderer, temp, NULL, &rect);
}

void SDLView::postFrame()
{
    SDL_RenderPresent(renderer);
}

Json::Value SDLView::getRoot(std::string filename)
{
    Json::Value root;
    Json::Reader reader;
    ifstream file;
    file.open(filename.c_str());
    if(!file.is_open())
    {
        Event_System::getSingleton().queueEvent(EventPtr(new MsgEvt("file " + filename + " cannot be opened for Json parsing")));
        return Json::Value(false);
    }
    file.seekg(0, file.end);
    int len = file.tellg();
    file.seekg(0, file.beg);
    char *dat = new char[len];
    file.read(dat, len);
    std::string fileStr(dat);
    file.close();
    if(!reader.parse(fileStr, root))
    {
        Event_System::getSingleton().queueEvent(EventPtr(new MsgEvt("JSON parser cannot parse the file: " + filename + "\n\treason:" + reader.getFormatedErrorMessages())));
        return Json::Value(false);
    }
    return root;
}
