import type {ForgeConfig} from '@electron-forge/shared-types';
import {MakerSquirrel} from '@electron-forge/maker-squirrel';
import {MakerZIP} from '@electron-forge/maker-zip';
import {MakerDeb} from '@electron-forge/maker-deb';
import {MakerRpm} from '@electron-forge/maker-rpm';
import {AutoUnpackNativesPlugin} from '@electron-forge/plugin-auto-unpack-natives';
import {FusesPlugin} from '@electron-forge/plugin-fuses';
import {FuseV1Options, FuseVersion} from '@electron/fuses';
import {WebpackPlugin as ForgeWebpackPlugin} from '@electron-forge/plugin-webpack';
import {mainConfig} from './webpack.main.config';
import {rendererConfig} from './webpack.renderer.config';
import {spawnSync} from 'node:child_process';
import * as path from 'node:path';

const config: ForgeConfig = {
    packagerConfig: {
        asar: true,
        extraResource: ["./resources/bin/", "./resources/config", "./resources/static"]
    },
    hooks: {
        generateAssets: async () => {
            const script = path.join(__dirname, "scripts", "copy-artifacts.js");
            console.log("Running copy-artifacts.js...");
            const result = spawnSync("node", [script], {stdio: "inherit"});
            if (result.status !== 0) throw new Error(`Failed to copy artifacts. Error: ${result.error}`);
        },
    },
    rebuildConfig: {},
    makers: [
        new MakerSquirrel({}),
        new MakerZIP({}, ['darwin']),
        new MakerRpm({}),
        new MakerDeb({}),
    ],
    plugins: [
        new AutoUnpackNativesPlugin({}),
        new ForgeWebpackPlugin({
            mainConfig,
            renderer: {
                config: rendererConfig,
                entryPoints: [
                    {
                        html: './src/index.html',
                        js: './src/renderer.ts',
                        name: 'main_window',
                        preload: {js: './src/preload.ts'},
                    },
                ],
            },
        }),
        new FusesPlugin({
            version: FuseVersion.V1,
            [FuseV1Options.RunAsNode]: false,
            [FuseV1Options.EnableCookieEncryption]: true,
            [FuseV1Options.EnableNodeOptionsEnvironmentVariable]: false,
            [FuseV1Options.EnableNodeCliInspectArguments]: false,
            [FuseV1Options.EnableEmbeddedAsarIntegrityValidation]: true,
            [FuseV1Options.OnlyLoadAppFromAsar]: true,
        }),
    ],
};

export default config;