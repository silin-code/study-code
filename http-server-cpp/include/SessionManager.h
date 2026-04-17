#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <string>
#include <sstream>
#include <unordered_map>
#include <mutex>
#include <ctime>
#include <random>

/*
 * Session里存的用户数据结构
 * 你可以在这里加你需要的用户信息，比如用户ID、权限等级等
 */
struct SessionData
{
    std::string username; // 用户名（示例数据，登录时用）
    time_t expireTime;    // Session的过期时间戳，到时间就作废
};

/*
 * 为什么要用单例模式？
 * 整个服务器里，Session池只能有一个，所有请求都共用这一个「客户信息表」
 * 单例模式能保证全局只有一个实例，不会重复创建多个Session池
 */
class SessionManager
{
public:
    // 获取全局唯一的SessionManager实例
    static SessionManager& getInstance();

    // 禁止拷贝和赋值，防止别人复制出多个实例
    SessionManager(const SessionManager &) = delete;
    SessionManager &operator=(const SessionManager &) = delete;

    // 创建新的Session（用户登录时调用）
    // 参数：username是用户名，timeout是Session过期时间（秒，默认1小时）
    // 返回：生成的唯一SessionID（给Cookie用的）
    std::string createSession(const std::string &username, int timeout = 3600);

    // 校验Session是否有效（用户访问页面时调用）
    // 参数：sessionId是从Cookie里拿到的编号
    // 返回：true表示Session有效，用户已登录；false表示无效/过期
    bool isSessionValid(const std::string &sessionId);

    // 从事Session里面获取用户名(登录后显示信息用)
    std::string getUsername(const std::string &sessionId);

    // 销毁Session(用户登出时调用，把服务器里的用户信息删掉)
    void destroySession(const std::string &sessionId);

    // 清理所有过期的Session(防止Session池越来越大，占内存)
    void clearExpiredsession();

private:
    //私有构造函数,外部不能new，只能通过getInstance()获取
    SessionManager() = default;

    //生成唯一的SessionId(时间戳+随机数，保证不重复)
    std::string generateSessionId();

    // 服务器的Session池（客户信息表）
    // key：SessionID（小票编号），value：用户信息+过期时间
    std::unordered_map<std::string ,SessionData> sessions_;

    //互斥锁，多线程处理时，防止多个线程同时修改sessions_导致数据错误;
    std::mutex mutex_;
};

#endif