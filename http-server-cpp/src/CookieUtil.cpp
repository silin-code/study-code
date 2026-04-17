#include "../include/CookieUtil.h"
#include <sstream>
#include <algorithm>

/*
 * 解析Cookie：从HTTP请求头里找"Cookie: "字段，然后拆成键值对
 * 举个例子，浏览器发的请求里有这么一段：
 * Cookie: SESSIONID=abc123; username=test
 * 这个函数会把它解析成：{"SESSIONID": "abc123", "username": "test"}
 */

// 解析请求里的所有Cookie
std::unordered_map<std::string, std::string> CookieUtil::parseCookies(const std::string &request)
{
    std::unordered_map<std::string, std::string> cookieMap;

    // 1:找请求头的"Cookie:" 行
    size_t cookiePos = request.find("Cookie:");
    // npos表示string查找失败的返回值通常是-1
    if (cookiePos == std::string::npos)
        return cookieMap;

    // 2:找到这一行的结尾(HTTP头每行以\r\n结束)
    size_t endPos = request.find("\r\n", cookiePos);
    if (endPos == std::string::npos)
        return cookieMap;

    // 3:取出Cookie的内容，比如"SESSION=abc123;username=test"
    std::string cookieStr = request.substr(cookiePos + 8, endPos - (cookiePos + 8));

    // 4::按照分好分割成键值对
    std::stringstream ss(cookieStr);
    std::string pair;
    while (std::getline(ss, pair, ';'))
    {
        // 去掉前后空格
        pair.erase(0, pair.find_first_not_of(" "));
        pair.erase(pair.find_last_not_of(" ")+1);
        if (pair.empty())
            continue;

        // 按照等号划分key和value
        size_t eqPos = pair.find("=");
        if (eqPos == std::string::npos)
            continue;

        std::string key = pair.substr(0, eqPos);
        std::string value = pair.substr(eqPos + 1);
        cookieMap[key] = value;
    }

    return cookieMap;
}

/*
 * 构建Set-Cookie响应头（服务器给浏览器发小票）
 * 每个参数的作用（新手必懂）：
 * - key/value：Cookie的名字和值，比如SESSIONID=abc123
 * - Path=/：表示这个Cookie对网站所有路径都有效
 * - HttpOnly：禁止JS读取Cookie，防止黑客偷Cookie（防XSS攻击，安全必备）
 * - Max-Age=3600：Cookie1小时后过期，浏览器会自动删掉
 */
// 构建Set-Cookie 响应头
std::string CookieUtil::buildSetCookie(const std::string &key, const std::string &value, int maxAge)
{
    // path=/: 这个cookie在整个网站都有效
    // HttpOnly: 禁止 JS 读取Cookie，防止 XSS 偷Cookie
    // Max-Age: Cookie有效期,单位秒，过期自动删除

    return "Set-Cookie:" + key + "=" + value +
           "; Path=/; HttpOnly; Max-Age=" + std::to_string(maxAge) + "\r\n";
}

/*
 * 清除Cookie：把Cookie的过期时间设为0，浏览器收到就会立刻删掉这个Cookie
 * 登出的时候调用，让浏览器的小票作废
 */
// 清楚Cookie:把有效期设为0，浏览器立刻删除
std::string CookieUtil::buildClearCookie(const std::string &key)
{
    return "Set-Cookie:" + key + "=; Path=/; HttpOnly; Max-Age=0\r\n";
}
