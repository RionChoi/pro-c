/**
 * Mail Service
 * 
 * This service handles outgoing emails. Currently, it logs to the console in development,
 * but it is designed to be easily connected to Resend, SendGrid, or an SMTP server.
 */

export interface SendMailOptions {
  to: string;
  subject: string;
  html: string;
  text?: string;
}

export async function sendMail(options: SendMailOptions): Promise<{ success: boolean; messageId?: string }> {
  const { to, subject, html } = options;

  // In a real implementation, you would use a library like 'nodemailer' or 'resend'.
  // Example for Resend:
  // const { data, error } = await resend.emails.send({
  //   from: 'Platform <noreply@platform.com>',
  //   to,
  //   subject,
  //   html,
  // });

  console.log(`[mail] Sending email to: ${to}`);
  console.log(`[mail] Subject: ${subject}`);
  console.log(`[mail] Content: ${html.substring(0, 100)}...`);

  // Simulate success
  return {
    success: true,
    messageId: `msg_${Math.random().toString(36).substring(7)}`,
  };
}

/**
 * Sends a member invitation email.
 */
export async function sendInviteEmail(to: string, inviteUrl: string, tenantName: string, role: string) {
  return sendMail({
    to,
    subject: `Invitation to join ${tenantName} on Platform`,
    html: `
      <h1>You've been invited!</h1>
      <p>You have been invited to join <strong>${tenantName}</strong> as a <strong>${role}</strong>.</p>
      <p>Click the link below to accept the invitation:</p>
      <a href="${inviteUrl}">${inviteUrl}</a>
      <p>If you didn't expect this, you can safely ignore this email.</p>
    `,
  });
}
