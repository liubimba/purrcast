export type FluxAction<T extends string = string, P = any> = {
    type: T;
    payload?: P;
};

type Callback = (action: FluxAction) => void;

class Dispatcher {
    private _callbacks: Callback[] = [];

    register(cb: Callback): Dispatcher {
        this._callbacks.push(cb);
        return this;
    }

    dispatch(action: FluxAction): void {
        for (const cb of this._callbacks) {
            cb(action);
        }
    }
}

export const dispatcher = new Dispatcher();