import { EventEmitter } from 'events';

export type BusMessage = { kind: 'device' | 'event' | 'alert'; deviceId: string; data: unknown };

export class Bus {
  private em = new EventEmitter();
  constructor() { this.em.setMaxListeners(0); }
  publish(m: BusMessage) { this.em.emit('msg', m); }
  subscribe(fn: (m: BusMessage) => void): () => void {
    this.em.on('msg', fn);
    return () => this.em.off('msg', fn);
  }
}
