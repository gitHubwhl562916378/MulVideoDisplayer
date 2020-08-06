#ifndef VIDEOENCODEBYFFMPEG_H
#define VIDEOENCODEBYFFMPEG_H

#include "servicei.h"
class VideoEncodeByFFmpeg : public VideoEncodeI
{
    Q_OBJECT
public:
    VideoEncodeByFFmpeg(QObject *parent = nullptr);
    ~VideoEncodeByFFmpeg();
    void startEncode(const EncodeParams& params) override;
    void stopEncode() override;

protected:
    void run() override;

private:
    static bool isInited_;
    EncodeParams params_;
};

#endif // VIDEOENCODEBYFFMPEG_H
