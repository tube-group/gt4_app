// redis_subscriber_get_value.cpp
#include <iostream>
#include <sw/redis++/redis++.h>

int main() {
    std::cout << "Redis监听器 - 获取key对应的value" << std::endl;
    std::cout << "按Ctrl+C退出" << std::endl;
    
    try {
        // 1. 连接Redis
        sw::redis::ConnectionOptions opts;
        opts.host = "140.32.1.192";
        opts.port = 6379;
        opts.password = "ggl2e=mc2";
        
        sw::redis::Redis redis(opts);
        
        std::cout << "连接成功" << std::endl;
        
        // 2. 订阅主题
        auto subscriber = redis.subscriber();
        subscriber.subscribe("RealDataChanged");
        
        std::cout << "已订阅RealDataChanged" << std::endl;
        
        // 3. 设置消息处理函数
        subscriber.on_message([&redis](std::string channel, std::string key) 
        {

            std::cout << "\n收到通知:" << std::endl;
            std::cout << "  频道: " << channel << std::endl;
            std::cout << "  键名: " << key << std::endl;
            
            // 根据key获取最新的value
            auto value = redis.get(key);
            
            if (value) {
                std::cout << "  值: " << *value << std::endl;
            } else {
                std::cout << "  警告: 键 '" << key << "' 不存在或为空" << std::endl;
            }
            
            std::cout << std::string(40, '-') << std::endl;
        });
        
        // 4. 开始监听
        while (true) {
            subscriber.consume();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}