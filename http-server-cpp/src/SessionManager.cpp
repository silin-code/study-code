#include "../include/SessionManager.h"

/*
 * 单例模式的核心：静态局部变量，第一次调用时初始化，之后一直用这个实例
 * 这是C++里线程安全的单例写法，新手不用深究，记住这个模板就行
 */

SessionManager& SessionManager::getInstance()
{
    static SessionManager instance;
    return instance;
}

/*
 * 生成唯一的SessionID：时间戳+16位随机数，保证不会重复
 * 为什么不用纯随机？防止两个用户同时登录生成相同的ID
 */
std::string SessionManager::generateSessionId()
{
    // 1获取当前时间戳(秒),确保时间上不重复
    time_t now = time(nullptr);
    std::stringstream ss;
    ss << now << "_";

    //2生成16位十六进制随机数，保证随机不重复
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0,15);
    for(int i=0;i<16;i++)
    {
        ss<<std::hex<<dis(gen);
    }
    return ss.str();
}

/*
 * 创建Session：用户登录时调用，生成一个新的Session存在服务器里
 * 用std::lock_guard自动加锁，函数结束自动解锁，不用手动管理锁
 */
std::string SessionManager::createSession(const std::string& username,int timeout)
{
    //枷锁，防止多个用户同时登录，同时修改session_导致数据错乱
    std::lock_guard<std::mutex> lock(mutex_);

    //1生成唯一sessionId   
    std::string sessionId =generateSessionId();

    //2填充session数据:用户名+过期时间(当前时间+timeout秒)
    SessionData data;
    data.username = username;
    data.expireTime = time(nullptr) +timeout;

    //3存到服务器的Session池里
    sessions_[sessionId] =data;

    return sessionId;
}
/*
 * 校验Session是否有效：用户每次访问页面时，都要先校验Session
 * 会自动清理过期的Session，不用单独调用clearExpiredSessions
 */
bool SessionManager::isSessionValid(const std::string& sessionId)
{
    std::lock_guard<std::mutex> lock(mutex_);

    //1先看SessionId在不在池子里
    auto it =sessions_.find(sessionId);
    if(it==sessions_.end())
    {
        return false;
    }

    //2再看Session有没有过期
    if(it->second.expireTime<time(nullptr))
    {
        //过期了
        sessions_.erase(it);
        return false;
    }

    //没过期
    return true;
}

/*
 * 从Session里获取用户名，登录后显示欢迎信息用
 */
std::string SessionManager::getUsername(const std::string& sessionId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if(sessions_.count(sessionId))
    {
        return sessions_[sessionId].username;
    }
    return "";
}

/*
 * 销毁Session：用户登出时调用，把服务器里的Session删掉
 */
void SessionManager::destroySession(const std::string& sessionId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(sessionId);
}

/*
 * 清理所有过期的Session：防止Session池越来越大，占内存
 * 你可以在服务器启动后开个定时线程调用这个方法，这里我们简化处理，在校验时自动清理
 */
void SessionManager::clearExpiredsession()
{
    std::lock_guard<std::mutex> lock(mutex_);
    time_t now = time(nullptr);
    for (auto it = sessions_.begin(); it != sessions_.end();) {
        if (it->second.expireTime < now) {
            // 过期了，删掉
            it = sessions_.erase(it);
        } else {
            ++it;
        }
    }
}

