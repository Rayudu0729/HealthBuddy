export interface SmsAdapter {
  send(msg: { to: string; content: string; requestId: string }): Promise<void>;
}

export class FakeSmsAdapter implements SmsAdapter {
  sent: { to: string; content: string; requestId: string }[] = [];
  async send(msg: { to: string; content: string; requestId: string }) {
    this.sent.push(msg);
    console.log(`[fake-sms] to=${msg.to} "${msg.content}"`);
  }
}

export class HttpSmsAdapter implements SmsAdapter {
  constructor(private apiKey: string, private from: string) {}
  async send({ to, content, requestId }: { to: string; content: string; requestId: string }) {
    const res = await fetch('https://api.httpsms.com/v1/messages/send', {
      method: 'POST',
      headers: { 'x-api-key': this.apiKey, 'Content-Type': 'application/json' },
      body: JSON.stringify({ from: this.from, to, content, request_id: requestId }),
    });
    if (!res.ok) throw new Error(`httpSMS ${res.status}: ${(await res.text()).slice(0, 200)}`);
  }
}
