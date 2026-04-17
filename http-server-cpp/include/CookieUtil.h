#ifndef COOKIEUTIL_H
#define COOKIEUTIL_H

#include <string>
#include <unordered_map>

// ########################### 新手注释 ###########################
// Cookie 工具类：专门处理 HTTP 协议里f的 Cookie
// 功能1：解析浏览器发过来的 Cookie
// 功能2：构建服务器发给浏览器的 Set-Cookie 响应头
// ################################################################
 /* 为什么要单独写CookieUtil？
 * 1. 解耦：服务器不用管Cookie怎么解析，只需要调用工具类的方法
 * 2. 可复用：以后其他项目需要处理Cookie，直接复制这个类就行
 * 3. 好维护：Cookie的格式变了，只改这个类，不影响服务器
 */
class CookieUtil {
public:
    // 解析请求头里的所有 Cookie，返回键值对
    // 参数：request 就是浏览器发给服务器的完整HTTP请求
    // 返回：键值对形式的Cookie，比如{"SESSIONID": "abc123"}
   static std::unordered_map<std::string, std::string> parseCookies(const std::string& request);

    // 构建Set-Cookie响应头
    // 参数：key是Cookie的名字（比如SESSIONID），value是值（SessionID），maxAge是过期时间（秒）
    // 比如构建出来的结果是：Set-Cookie: SESSIONID=abc123; Path=/; HttpOnly; Max-Age=3600
    static std::string buildSetCookie(const std::string& key, const std::string& value, int maxAge = 3600);

    // 构建清除 Cookie 的响应头（让浏览器删掉这个 Cookie）
    static std::string buildClearCookie(const std::string& key);
};

#endif