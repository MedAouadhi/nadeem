export function ErrorCard({ message = "حدث خطأ أثناء تحميل البيانات" }: { message?: string }) {
  return (
    <div className="bg-error-container/20 rounded-xl p-8 text-center">
      <span className="material-symbols-outlined text-4xl text-error mb-2 block">error</span>
      <p className="text-on-error-container font-bold mb-1">عذراً</p>
      <p className="text-on-surface-variant font-body">{message}</p>
    </div>
  );
}
