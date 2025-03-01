import wx


class MainDialog(wx.Frame):
    def __init__(self, parent, title, url, version, open_callback, close_callback, later_callback, skip_callback):
        super(MainDialog, self).__init__(parent, title=title, size=(400, 220))

        self.close_callback = close_callback
        self.later_callback = later_callback
        self.skip_callback = skip_callback
        self.open_callback = open_callback

        self.url = url
        self.description = f"ikulab-motion-viewer {version}がリリースされました。\n下記のリンクからダウンロードしてください。"

        self.panel = wx.Panel(self)
        self.main_sizer = wx.BoxSizer(wx.VERTICAL)
        self.panel.SetSizer(self.main_sizer)

        # init dialog ui parts
        self.init_ui()

        # bind on close event
        self.Bind(wx.EVT_CLOSE, self.on_close)

    def init_ui(self):
        description_label = wx.StaticText(self.panel, label=self.description)
        self.main_sizer.Add(description_label, 0, wx.ALIGN_CENTER | wx.TOP, 20)

        url_label = wx.StaticText(self.panel, label=self.url)
        self.main_sizer.Add(url_label, 0, wx.ALIGN_CENTER | wx.TOP, 20)

        open_button = wx.Button(self.panel, label="ブラウザで開く")
        open_button.Bind(wx.EVT_BUTTON, self.on_open)
        open_button.SetDefault()
        open_button.SetFocus()
        self.main_sizer.Add(open_button, 0, wx.ALIGN_CENTER | wx.TOP, 10)

        self.main_sizer.AddStretchSpacer()

        bottom_btn_sizer = self.create_bottom_buttons_sizer()
        self.main_sizer.Add(bottom_btn_sizer, 0, wx.ALIGN_CENTER | wx.BOTTOM, 20)

    def create_bottom_buttons_sizer(self):
        sizer = wx.BoxSizer(wx.HORIZONTAL)

        later_button = wx.Button(self.panel, label="後で")
        later_button.Bind(wx.EVT_BUTTON, self.on_later)
        sizer.Add(later_button, 0, wx.ALIGN_CENTER | wx.RIGHT, 20)

        skip_button = wx.Button(self.panel, label="このバージョンをスキップ")
        skip_button.Bind(wx.EVT_BUTTON, self.on_skip)
        sizer.Add(skip_button, 0, wx.ALIGN_CENTER | wx.LEFT, 20)

        return sizer

    def on_open(self, _):
        self.open_callback()
        self.Destroy()

    def on_close(self, _):
        self.close_callback()
        self.Destroy()

    def on_later(self, _):
        self.later_callback()
        self.Destroy()

    def on_skip(self, _):
        self.skip_callback()
        self.Destroy()


class MainUi(wx.App):
    def __init__(self, latest_version, open_callback, close_callback, later_callback, skip_callback):
        self.latest_version = latest_version
        self.open_callback = open_callback
        self.close_callback = close_callback
        self.later_callback = later_callback
        self.skip_callback = skip_callback

        super().__init__(redirect=False, filename=None, useBestVisual=True, clearSigInt=True)

    def OnInit(self):
        frame = MainDialog(None,
                           "ikulab-motion-viewer 更新",
                           "https://github.com/ikulab/ikulab-motion-viewer/releases/latest",
                           self.latest_version,
                           self.open_callback,
                           self.close_callback,
                           self.later_callback,
                           self.skip_callback)
        self.SetTopWindow(frame)
        frame.Show(True)
        frame.Raise()
        frame.SetFocus()

        return True
