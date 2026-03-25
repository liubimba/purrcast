![preview image](docs/preview.png)

# MULTIROOM

![platform](https://img.shields.io/badge/platform-Ubuntu-blue)
![license](https://img.shields.io/badge/license-MIT-green)

**MULTIROOM** --- Десктоп приложение, которое превращает любое
устройство в вашей локальной сети в синхронизированную колонку через
браузер.

Приложение захватывает системный звук, распространяет его по сети и
синхронно воспроизводит на всех подключённых устройствах.

------------------------------------------------------------------------

# Table of Contents

-   [Overview](#overview)
-   [Features](#features)
-   [Quick Start](#quick-start)
-   [Installation](#installation)
-   [Usage](#usage)
-   [Development](#development)
-   [Roadmap](#roadmap)
-   [Contributing](#contributing)
-   [License](#license)

------------------------------------------------------------------------

# Overview

`MULTIROOM` позволяет:

-   захватывать системный звук
-   синхронно передавать его на другие устройства
-   управлять громкостью каждого клиента
-   управлять общей громкостью системы
-   мониторить состояние сервисов

Любое устройство с браузером в локальной сети может стать колонкой.

------------------------------------------------------------------------

# Features

-   синхронизированный multiroom звук
-   web UI для управления
-   индивидуальная громкость клиента
-   master громкость
-   mute / unmute
-   мониторинг сервисов
-   автоматический запуск сервисов

------------------------------------------------------------------------

# Quick Start

1.  Установите приложение.
2.  Запустите **multiroom**.
3.  Откроется панель управления.

Другие устройства могут подключиться через браузер:

http://`<host-ip>`:3000

Все подключённые устройства будут воспроизводить звук синхронно.

------------------------------------------------------------------------

# Installation

Пока поддерживается только **Ubuntu**.

Установите зависимости:

``` bash
sudo apt-get update -qq

sudo apt-get install -y \
  libboost-all-dev \
  libpulse-dev \
  libasound2-dev \
  pkg-config \
  libavahi-client-dev \
  libflac-dev \
  libogg-dev \
  libvorbis-dev \
  libopus-dev
```

Установите приложение через **Electron installer**.

------------------------------------------------------------------------

# Usage

После запуска приложения:

1.  Electron запускает backend сервис.
2.  Открывается Web UI.
3.  Система начинает захват аудио.

В интерфейсе доступны:

### Client Controls

-   громкость клиента
-   mute / unmute
-   имя устройства

### Master Controls

-   master volume
-   mute / unmute всех клиентов

### Monitoring

отображается состояние:

-   snapserver
-   snapclient
-   loopback
-   routing

# Development

### Build backend

``` bash
go build
```

### Build C++ launcher

``` bash
cmake .
make
```

### Run frontend

``` bash
npm install
npm run dev
```

------------------------------------------------------------------------

# Roadmap

Планируемые улучшения:

-   macOS support
-   Windows support
-   улучшенная диагностика
-   расширенный мониторинг

------------------------------------------------------------------------

# Contributing

Pull requests приветствуются.

Если вы хотите внести вклад:

1.  Fork репозитория
2.  Создайте feature branch
3.  Сделайте изменения
4.  Откройте Pull Request

------------------------------------------------------------------------

# License

Проект распространяется под лицензией MIT.
