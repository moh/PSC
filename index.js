const electron = require("electron");

const { app, BrowserWindow } = electron;


const creatWindow = () => {
  const win = new BrowserWindow(
    {
      width : 800,
      height: 600,
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