const electron = require("electron");

const { app, BrowserWindow } = electron;


const creatWindow = () => {
  const win = new BrowserWindow(
    {
      width : 1100,
      height: 700,
    }
  )
  win.loadFile('index.html');
  win.webContents.openDevTools();
}

app.whenReady().then(() => {
  creatWindow()
})

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit()
})