import json
import urllib.request

import requests

# get latest version of ikulab-motion-viewer from GitHub
# returns (succeeded, latest_version)
def get_latest_release_version():
    url = "https://api.github.com/repos/ikulab/ikulab-motion-viewer/releases/latest"

    try:
        response = requests.get(url)
        response.raise_for_status()
        json_body = response.json()
        return True, json_body["tag_name"]
    except requests.RequestException:
        return False, ""
