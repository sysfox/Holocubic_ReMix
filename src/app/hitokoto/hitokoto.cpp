#include "hitokoto.h"
#include "hitokoto_gui.h"
#include "sys/app_controller.h"
#include "network.h"
#include "common.h"
#include "esp_log.h"
#include "HTTPClient.h"
#include "SPIFFS.h"

#define HITOKOTO_API_URL "https://api.trfox.top/public/image/holocubic/hitokoto"
#define HITOKOTO_CACHE_FILE "/hitokoto_cache.jpg"
#define HITOKOTO_DOWNLOAD_TIMEOUT 10000  // 下载超时时间，单位毫秒

static const char *TAG = "Hitokoto";

// 应用图标
extern const lv_img_dsc_t hitokoto_ico;

// 创建应用
Hitokoto::Hitokoto()
{
    // 设置APP名称
    app_name = "Hitokoto";
    app_image = &hitokoto_ico;

    // 默认不开启休眠
    deepsleep_time = 0;
}

Hitokoto::~Hitokoto()
{
}

void Hitokoto::app_init(void)
{
    // 初始化GUI
    hitokoto_gui_init();
    // 加载图片
    load_image();
}

void Hitokoto::load_image(void)
{
    File file;
    bool use_cache = false;
    
    // 显示加载动画
    hitokoto_gui_show_loading(true);
    hitokoto_gui_set_status_text("连接网络...");
    
    // 检查网络连接
    if (WiFi.status() != WL_CONNECTED) {
        ESP_LOGE(TAG, "WiFi not connected");
        hitokoto_gui_set_status_text("网络未连接");
        hitokoto_gui_show_loading(false);
        
        // 尝试从缓存加载
        if (SPIFFS.exists(HITOKOTO_CACHE_FILE)) {
            hitokoto_gui_set_status_text("从缓存加载...");
            file = SPIFFS.open(HITOKOTO_CACHE_FILE, "r");
            if (file) {
                use_cache = true;
            }
        }
        
        if (!use_cache) {
            return;
        }
    }
    
    if (!use_cache) {
        // 下载图片
        hitokoto_gui_set_status_text("下载图片...");
        
        HTTPClient http;
        http.begin(HITOKOTO_API_URL);
        http.setTimeout(HITOKOTO_DOWNLOAD_TIMEOUT);
        
        int httpCode = http.GET();
        if (httpCode != HTTP_CODE_OK) {
            ESP_LOGE(TAG, "HTTP GET failed, error: %s", http.errorToString(httpCode).c_str());
            hitokoto_gui_set_status_text("下载失败");
            hitokoto_gui_show_loading(false);
            
            // 尝试从缓存加载
            if (SPIFFS.exists(HITOKOTO_CACHE_FILE)) {
                hitokoto_gui_set_status_text("从缓存加载...");
                file = SPIFFS.open(HITOKOTO_CACHE_FILE, "r");
                if (file) {
                    use_cache = true;
                } else {
                    return;
                }
            } else {
                return;
            }
        } else {
            // 下载成功，写入缓存文件
            int len = http.getSize();
            ESP_LOGI(TAG, "Image size: %d bytes", len);
            
            if (len <= 0) {
                ESP_LOGE(TAG, "Invalid image size");
                hitokoto_gui_set_status_text("图片无效");
                hitokoto_gui_show_loading(false);
                http.end();
                return;
            }
            
            // 写入缓存
            file = SPIFFS.open(HITOKOTO_CACHE_FILE, "w");
            if (!file) {
                ESP_LOGE(TAG, "Failed to open cache file for writing");
            } else {
                WiFiClient *stream = http.getStreamPtr();
                
                // 分配足够的缓冲区
                const size_t bufferSize = 1024;
                uint8_t *buffer = (uint8_t *)malloc(bufferSize);
                if (!buffer) {
                    ESP_LOGE(TAG, "Failed to allocate memory for buffer");
                    hitokoto_gui_set_status_text("内存不足");
                    hitokoto_gui_show_loading(false);
                    file.close();
                    http.end();
                    return;
                }
                
                // 读取并写入文件
                size_t totalBytes = 0;
                while (http.connected() && (totalBytes < len)) {
                    size_t available = stream->available();
                    if (available) {
                        size_t readBytes = stream->readBytes(buffer, min(available, bufferSize));
                        file.write(buffer, readBytes);
                        totalBytes += readBytes;
                    }
                    
                    // 避免看门狗复位
                    delay(1);
                }
                
                free(buffer);
                file.close();
                
                if (totalBytes != len) {
                    ESP_LOGW(TAG, "Incomplete download: %d of %d bytes", totalBytes, len);
                    hitokoto_gui_set_status_text("下载不完整");
                }
                
                // 重新打开文件用于显示
                file = SPIFFS.open(HITOKOTO_CACHE_FILE, "r");
            }
            
            http.end();
        }
    }
    
    // 显示图片
    if (file) {
        size_t fileSize = file.size();
        ESP_LOGI(TAG, "File size: %d bytes", fileSize);
        
        if (fileSize > 0) {
            // 分配足够的内存来存储图片
            uint8_t *imgBuffer = (uint8_t *)malloc(fileSize);
            if (imgBuffer) {
                size_t bytesRead = file.read(imgBuffer, fileSize);
                if (bytesRead == fileSize) {
                    hitokoto_gui_set_image(imgBuffer, fileSize);
                    hitokoto_gui_set_status_text("一言图片");
                } else {
                    ESP_LOGE(TAG, "Read error: %d of %d bytes", bytesRead, fileSize);
                    hitokoto_gui_set_status_text("读取错误");
                }
                free(imgBuffer);
            } else {
                ESP_LOGE(TAG, "Failed to allocate memory for image");
                hitokoto_gui_set_status_text("内存不足");
            }
        } else {
            ESP_LOGE(TAG, "Empty file");
            hitokoto_gui_set_status_text("文件为空");
        }
        
        file.close();
    }
    
    // 隐藏加载动画
    hitokoto_gui_show_loading(false);
}

void Hitokoto::app_process(AppController *sys_app)
{
    lv_scr_load_anim_t anim_type = LV_SCR_LOAD_ANIM_FADE_ON;
    if (FIRST_RUNNING == app_run_state) {
        app_run_state = RUNNING;
    }
    
    // 进入省电模式前的操作
    if (deepsleep_time != 0 && millis() - run_time >= deepsleep_time * 1000) {
        sys_app->app_exit();
        sys_app->power_mgmt_sleep(deepsleep_time);
    }
}

void Hitokoto::app_exit_callback(void)
{
    // 清理GUI相关内容
    hitokoto_gui_delete();
}

void Hitokoto::on_btn_update(void)
{
    // 刷新按钮处理
    ESP_LOGI(TAG, "Update button pressed, refreshing image");
    load_image();
} 