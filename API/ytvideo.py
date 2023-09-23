#!/usr/bin/python3
import yt_dlp
import subprocess
import cli_to_api
import sys

videoid = sys.argv[1]


if videoid is None:
    print(f"videoid = {videoid}")
    exit(2)
else:
    videoid = f"https://youtu.be/{videoid}"

# тихий режим - -q
ytdl_options = cli_to_api.cli_to_api(sys.argv[2:])


with yt_dlp.YoutubeDL(ytdl_options) as yt:
    try:
        info = yt.extract_info(videoid, download=False)
    except yt_dlp.utils.ExtractorError:
        exit(3)
    except yt_dlp.utils.DownloadError:
        exit(3)


    if ((info['is_live'] is None) or (info['is_live'] is False)): #video

        if(info['filesize'] > 104857600):
            exit(5)

        code = yt.download([videoid])
        exit(0)
    else: #stream
        subprocess.Popen(['yt-dlp'] + sys.argv[1:]) # ffmpeg stream fuck up
        exit(1)
