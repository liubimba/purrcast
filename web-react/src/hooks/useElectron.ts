export const useElectron = () => {
    return new URLSearchParams(window.location.search).get('electron') === "true";
}