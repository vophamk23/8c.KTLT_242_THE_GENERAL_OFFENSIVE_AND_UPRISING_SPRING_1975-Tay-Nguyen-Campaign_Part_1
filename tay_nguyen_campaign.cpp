#include "tay_nguyen_campaign.h"

using namespace std;

//--------------------------------------------------------------
// Các hàm trợ giúp nhỏ (Không dùng ?:, Không dùng vòng lặp)
//--------------------------------------------------------------

// Các hàm này được thiết kế để tránh sử dụng vòng lặp hoặc toán tử ba ngôi (?:)
// Thay vào đó, chúng sử dụng các điều kiện đơn giản và đệ quy

// 1) Hàm giới hạn - Đảm bảo một giá trị nằm trong phạm vi xác định
static int clampInt(int val, int low, int high)
{
    // Nếu giá trị nhỏ hơn giới hạn dưới, trả về giới hạn dưới
    // Nếu giá trị lớn hơn giới hạn trên, trả về giới hạn trên
    // Ngược lại trả về chính giá trị đó
    return (val < low) ? low : (val > high ? high : val);
}

// 2) Làm tròn lên cho double->int - Làm tròn lên một số thực thành số nguyên gần nhất
static int myCeil(double x)
{
    int xi = (int)x; // Ép kiểu sang int (cắt phần thập phân về phía số 0)
    // Nếu x dương và lớn hơn phần nguyên, cộng thêm 1
    // Ngược lại trả về giá trị đã cắt
    return (x > 0.0 && x > double(xi)) ? (xi + 1) : xi;
}

// 3) Min & Max - Trả về giá trị nhỏ nhất hoặc lớn nhất của hai số
static int myMin(int a, int b)
{
    return (a < b) ? a : b; // Trả về giá trị nhỏ hơn
}
static int myMax(int a, int b)
{
    return (a > b) ? a : b; // Trả về giá trị lớn hơn
}

// 4) Kiểm tra chữ cái & chữ số - Xác minh nếu một ký tự là chữ cái hoặc chữ số
static bool isAlpha(char c)
{
    // Trả về true nếu c là chữ cái viết hoa hoặc viết thường
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}
static bool isDigit(char c)
{
    // Trả về true nếu c là chữ số từ 0-9
    return (c >= '0' && c <= '9');
}

// 5) toUpper / toLower - Chuyển đổi kiểu chữ
static char toUpperChar(char c)
{
    if (c >= 'a' && c <= 'z')
    {
        // Chuyển chữ thường thành chữ hoa bằng cách trừ hiệu giữa 'a' và 'A'
        return char(c - ('a' - 'A'));
    }
    return c;
}
static char toLowerChar(char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        // Chuyển chữ hoa thành chữ thường bằng cách cộng hiệu giữa 'a' và 'A'
        return char(c + ('a' - 'A'));
    }
    return c;
}

//--------------------------------------------------------------
// Đệ quy thay thế cho vòng lặp
//--------------------------------------------------------------

//=== readFile đệ quy ===//

// Đọc chính xác 5 dòng
static bool readLinesRecursive(ifstream &ifs, string lines[], int i)
{
    if (i >= 5)
    {
        return true; // trường hợp cơ sở: đã đọc tất cả 5 dòng
    }
    if (!std::getline(ifs, lines[i]))
    {
        return false; // không thể đọc dòng tiếp theo
    }
    return readLinesRecursive(ifs, lines, i + 1); // đọc dòng tiếp theo
}

// Phân tích 17 số nguyên vào mảng LF[]
static bool parseLFRecursive(istringstream &iss, int LF[], int i)
{
    if (i >= 17)
    {
        return true; // hoàn thành
    }
    if (!(iss >> LF[i]))
    {
        return false; // không thể đọc số tiếp theo
    }
    LF[i] = clampInt(LF[i], 0, 1000); // giới hạn giá trị trong khoảng [0, 1000]

    if (i < 16)
    {
        // xử lý dấu phẩy có thể có
        iss >> std::ws; // bỏ qua khoảng trắng
        if (iss.peek() == ',')
        {
            char dummy;
            iss.get(dummy); // đọc và bỏ qua dấu phẩy
        }
    }
    return parseLFRecursive(iss, LF, i + 1); // xử lý số tiếp theo
}

//=== gatherForces đệ quy ===//

// Tính tổng của các phần tử LF1 và LF2 nhân với trọng số tương ứng
static int gatherForcesRec(int i, int LF1[], int LF2[], const int weight[])
{
    if (i >= 17)
    {
        return 0; // trường hợp cơ sở - đã xử lý hết các phần tử
    }
    if (LF1[i] < 0)
    {
        LF1[i] = 0; // đảm bảo không có giá trị âm
    }
    if (LF2[i] < 0)
    {
        LF2[i] = 0; // đảm bảo không có giá trị âm
    }
    int val = (LF1[i] + LF2[i]) * weight[i];               // tính giá trị cho phần tử hiện tại
    return val + gatherForcesRec(i + 1, LF1, LF2, weight); // cộng với giá trị của các phần tử còn lại
}

//=== determineRightTarget đệ quy ===//

// Trích xuất các chữ số từ chuỗi target và lưu vào mảng numbers
static void extractDigitsRec(const string &target, size_t i, int &count, int numbers[])
{
    if (i >= target.size())
    {
        return; // đã xử lý hết chuỗi
    }
    if (isDigit(target[i]))
    {
        int num = (target[i] - '0'); // chuyển ký tự số thành giá trị số
        size_t j = i + 1;
        // đọc các ký tự số tiếp theo để tạo thành số có nhiều chữ số
        while (j < target.size() && isDigit(target[j]))
        {
            num = num * 10 + (target[j] - '0'); // thêm chữ số vào bên phải
            j++;
        }
        if (count < 3)
        {
            numbers[count++] = num; // lưu số vào mảng nếu chưa đạt giới hạn
        }
        else
        {
            // đánh dấu không hợp lệ bằng cách đặt count thành một giá trị lớn
            count = 9999;
        }
        extractDigitsRec(target, j, count, numbers); // tiếp tục xử lý từ vị trí sau số
    }
    else
    {
        extractDigitsRec(target, i + 1, count, numbers); // bỏ qua ký tự không phải số
    }
}

//=== decodeTarget đệ quy ===//

// Lọc các ký tự hợp lệ từ chuỗi message
static void filterRec(const string &message, size_t i, string &filtered)
{
    if (i >= message.size())
    {
        return; // đã xử lý hết chuỗi
    }
    char c = message[i];
    if (isAlpha(c) || (c >= '0' && c <= '9') || (c == ' '))
    {
        filtered.push_back(c); // chỉ giữ lại chữ cái, chữ số và dấu cách
    }
    filterRec(message, i + 1, filtered); // xử lý ký tự tiếp theo
}

// Chuyển chuỗi thành dạng Title Case (viết hoa chữ cái đầu mỗi từ)
static void titleCaseRec(string &s, size_t i, bool capitalize)
{
    if (i >= s.size())
    {
        return; // đã xử lý hết chuỗi
    }
    if (isAlpha(s[i]))
    {
        if (capitalize)
        {
            s[i] = toUpperChar(s[i]); // viết hoa nếu cần
        }
        else
        {
            s[i] = toLowerChar(s[i]); // viết thường nếu không cần viết hoa
        }
        if (s[i] != ' ')
        {
            capitalize = false; // đặt lại biến cờ sau khi xử lý chữ cái đầu tiên của từ
        }
    }
    if (s[i] == ' ')
    {
        capitalize = true; // chuẩn bị viết hoa chữ cái đầu tiếp theo
    }
    titleCaseRec(s, i + 1, capitalize); // xử lý ký tự tiếp theo
}

// Mã hóa Caesar - dịch chuyển các chữ cái theo một số vị trí nhất định
static void caesarShiftRec(string &s, size_t i, int shift)
{
    if (i >= s.size())
    {
        return; // đã xử lý hết chuỗi
    }
    char c = s[i];
    if (c >= 'A' && c <= 'Z')
    {
        s[i] = char('A' + ((c - 'A' + shift) % 26)); // dịch chữ hoa
    }
    else if (c >= 'a' && c <= 'z')
    {
        s[i] = char('a' + ((c - 'a' + shift) % 26)); // dịch chữ thường
    }
    caesarShiftRec(s, i + 1, shift); // xử lý ký tự tiếp theo
}

// Đảo ngược chuỗi
static void reverseStringRec(string &s, size_t left, size_t right)
{
    if (left >= right)
    {
        return; // đã hoàn thành đảo ngược
    }
    char tmp = s[left];
    s[left] = s[right]; // hoán đổi ký tự trái và phải
    s[right] = tmp;
    reverseStringRec(s, left + 1, right - 1); // xử lý cặp ký tự tiếp theo
}

// Kiểm tra xem một chuỗi có trùng với một trong 5 mục tiêu chính không
static bool checkPrimaryTargetRec(const string &str, const string arr[], int index)
{
    if (index >= 5)
    {
        return false; // đã kiểm tra tất cả các mục tiêu, không tìm thấy trùng khớp
    }
    if (str == arr[index])
    {
        return true; // tìm thấy trùng khớp
    }
    return checkPrimaryTargetRec(str, arr, index + 1); // kiểm tra mục tiêu tiếp theo
}

//=== planAttack đệ quy ===//

// Tính toán giá trị cuối cùng cho kế hoạch tấn công
static double planAttackRec(int battleField[10][10], int i, int j, double S)
{
    if (i >= 10)
    {
        // đã xử lý tất cả các hàng
        return S;
    }
    if (j >= 10)
    {
        // chuyển sang hàng tiếp theo
        return planAttackRec(battleField, i + 1, 0, S);
    }
    // Tính toán giá trị ô hiện tại dựa trên vị trí hàng
    double cell = battleField[i][j] * ((((i + 1) % 2) == 1) ? (2.0 / 3.0) : (3.0 / 2.0));
    return planAttackRec(battleField, i, j + 1, S - cell); // trừ cell từ S và xử lý ô tiếp theo
}

//=== resupply đệ quy ===//

// Chuyển mảng 2 chiều thành mảng 1 chiều
static void flattenRec(int supply[5][5], int arr[], int i, int j, int &idx)
{
    if (i >= 5)
    {
        return; // đã xử lý hết mảng 2 chiều
    }
    if (j >= 5)
    {
        flattenRec(supply, arr, i + 1, 0, idx); // chuyển sang hàng tiếp theo
    }
    else
    {
        arr[idx++] = supply[i][j];              // sao chép giá trị vào mảng 1 chiều
        flattenRec(supply, arr, i, j + 1, idx); // xử lý phần tử tiếp theo trong hàng hiện tại
    }
}

// Sắp xếp chèn đệ quy - dịch chuyển phần tử
static void insertionShiftDown(int arr[], int j, int key)
{
    if (j < 0 || arr[j] <= key)
    {
        // trường hợp cơ sở - đã tìm thấy vị trí đúng
        arr[j + 1] = key;
        return;
    }
    arr[j + 1] = arr[j];                 // dịch phần tử lên một vị trí
    insertionShiftDown(arr, j - 1, key); // tiếp tục kiểm tra vị trí trước
}

// Sắp xếp chèn đệ quy - lặp qua các phần tử
static void insertionSortRec(int arr[], int n, int i)
{
    if (i >= n)
    {
        return; // đã sắp xếp tất cả phần tử
    }
    int key = arr[i]; // phần tử cần chèn
    int j = i - 1;
    insertionShiftDown(arr, j, key); // tìm vị trí và chèn key
    insertionSortRec(arr, n, i + 1); // sắp xếp phần tử tiếp theo
}

// Kiểm tra một tổ hợp 5 phần tử
static void checkCombo(int arr[], int shortfall, int &minSum,
                       int a, int b, int c, int d, int e)
{
    int sum = arr[a] + arr[b] + arr[c] + arr[d] + arr[e]; // tính tổng của 5 phần tử
    if (sum >= shortfall && sum < minSum)
    {
        minSum = sum; // cập nhật tổng nhỏ nhất thỏa mãn yêu cầu
    }
}

// Các hàm lặp đệ quy để tạo tất cả các tổ hợp 5 phần tử từ mảng 25 phần tử
// loopE - lặp qua chỉ số thứ 5
static void loopE(int arr[], int shortfall, int &minSum,
                  int a, int b, int c, int d, int e)
{
    if (e >= 25)
    {
        return;
    }
    checkCombo(arr, shortfall, minSum, a, b, c, d, e); // kiểm tra tổ hợp hiện tại
    loopE(arr, shortfall, minSum, a, b, c, d, e + 1);  // thử với giá trị e tiếp theo
}

// loopD - lặp qua chỉ số thứ 4
static void loopD(int arr[], int shortfall, int &minSum,
                  int a, int b, int c, int d)
{
    if (d >= 24)
    {
        return;
    }
    loopE(arr, shortfall, minSum, a, b, c, d, d + 1); // thử với tất cả giá trị e có thể
    loopD(arr, shortfall, minSum, a, b, c, d + 1);    // thử với giá trị d tiếp theo
}

// loopC - lặp qua chỉ số thứ 3
static void loopC(int arr[], int shortfall, int &minSum,
                  int a, int b, int c)
{
    if (c >= 23)
    {
        return;
    }
    loopD(arr, shortfall, minSum, a, b, c, c + 1); // thử với tất cả giá trị d có thể
    loopC(arr, shortfall, minSum, a, b, c + 1);    // thử với giá trị c tiếp theo
}

// loopB - lặp qua chỉ số thứ 2
static void loopB(int arr[], int shortfall, int &minSum,
                  int a, int b)
{
    if (b >= 22)
    {
        return;
    }
    loopC(arr, shortfall, minSum, a, b, b + 1); // thử với tất cả giá trị c có thể
    loopB(arr, shortfall, minSum, a, b + 1);    // thử với giá trị b tiếp theo
}

// loopA - lặp qua chỉ số đầu tiên
static void loopA(int arr[], int shortfall, int &minSum, int a)
{
    if (a >= 21)
    {
        return;
    }
    loopB(arr, shortfall, minSum, a, a + 1); // thử với tất cả giá trị b có thể
    loopA(arr, shortfall, minSum, a + 1);    // thử với giá trị a tiếp theo
}

//--------------------------------------------------------------
// CÁC HÀM YÊU CẦU
//--------------------------------------------------------------

// 1) readFile - Đọc dữ liệu từ file
bool readFile(const string &filename,
              int LF1[], int LF2[],
              int &EXP1, int &EXP2,
              int &T1, int &T2, int &E)
{
    ifstream ifs(filename); // mở file để đọc
    if (!ifs.is_open())
    {
        return false; // không mở được file
    }

    {
        string lines[5]; // mảng chứa 5 dòng từ file
        // đọc 5 dòng
        if (!readLinesRecursive(ifs, lines, 0))
        {
            return false; // không đọc đủ 5 dòng
        }
        ifs.close(); // đóng file

        // phân tích dòng 1 -> LF1
        {
            istringstream iss(lines[0]); // tạo luồng để phân tích chuỗi
            char ch;
            if (!(iss >> ch) || ch != '[')
            {
                return false; // không tìm thấy dấu '[' mở đầu
            }
            if (!parseLFRecursive(iss, LF1, 0))
            {
                return false; // không phân tích được các số
            }
            iss >> std::ws; // bỏ qua khoảng trắng
            if (!(iss >> ch) || ch != ']')
            {
                return false; // không tìm thấy dấu ']' kết thúc
            }
        }
        // phân tích dòng 2 -> LF2
        {
            istringstream iss(lines[1]);
            char ch;
            if (!(iss >> ch) || ch != '[')
            {
                return false;
            }
            if (!parseLFRecursive(iss, LF2, 0))
            {
                return false;
            }
            iss >> std::ws;
            if (!(iss >> ch) || ch != ']')
            {
                return false;
            }
        }
        // phân tích dòng 3 -> EXP1, EXP2
        {
            istringstream iss(lines[2]);
            if (!(iss >> EXP1 >> EXP2))
            {
                return false; // không đọc được hai số
            }
            EXP1 = clampInt(EXP1, 0, 600); // giới hạn giá trị EXP1
            EXP2 = clampInt(EXP2, 0, 600); // giới hạn giá trị EXP2
        }
        // phân tích dòng 4 -> T1, T2
        {
            istringstream iss(lines[3]);
            if (!(iss >> T1 >> T2))
            {
                return false; // không đọc được hai số
            }
            T1 = clampInt(T1, 0, 3000); // giới hạn giá trị T1
            T2 = clampInt(T2, 0, 3000); // giới hạn giá trị T2
        }
        // phân tích dòng 5 -> E
        {
            istringstream iss(lines[4]);
            if (!(iss >> E))
            {
                return false; // không đọc được số
            }
            E = clampInt(E, 0, 99); // giới hạn giá trị E
        }
    }
    return true; // đọc và phân tích thành công
}

// 2) gatherForces - Tính toán tổng lực lượng
int gatherForces(int LF1[], int LF2[])
{
    int weight[17] = {1, 2, 3, 4, 5, 7, 8, 9, 10, 12, 15, 18, 20, 30, 40, 50, 70}; // trọng số cho từng loại lực lượng
    return gatherForcesRec(0, LF1, LF2, weight);                                   // tính tổng bằng đệ quy
}

// 3) determineRightTarget - Xác định mục tiêu đúng từ chuỗi đầu vào
string determineRightTarget(const string &target)
{
    const string PRIMARY_TARGETS[5] = {
        "Buon Ma Thuot",
        "Duc Lap",
        "Dak Lak",
        "National Route 21",
        "National Route 14"}; // danh sách các mục tiêu chính

    // 1) Trích xuất tối đa 3 số từ chuỗi
    int numbers[3];
    int count = 0;
    extractDigitsRec(target, 0, count, numbers);
    if (count > 3)
    {
        // không hợp lệ nếu có hơn 3 số
        return "INVALID";
    }

    // 2) Xác định mục tiêu dựa trên số lượng số đã trích xuất:

    // Nếu có 1 số
    if (count == 1)
    {
        int n = numbers[0];
        if (n >= 3 && n <= 7)
        {
            // => PRIMARY_TARGETS[n - 3]
            return PRIMARY_TARGETS[n - 3];
        }
        else if (n >= 0 && n <= 2)
        {
            // => "DECOY"
            return "DECOY";
        }
        else
        {
            return "INVALID";
        }
    }
    // Nếu có 2 số
    else if (count == 2)
    {
        int idx = (numbers[0] + numbers[1]) % 5;
        return PRIMARY_TARGETS[idx];
    }
    // Nếu có 3 số
    else if (count == 3)
    {
        int maxVal = numbers[0];
        if (numbers[1] > maxVal)
        {
            maxVal = numbers[1];
        }
        if (numbers[2] > maxVal)
        {
            maxVal = numbers[2];
        }
        return PRIMARY_TARGETS[maxVal % 5];
    }
    // Không có số nào
    return "INVALID";
}

// 4) decodeTarget - Giải mã thông điệp mục tiêu
string decodeTarget(const string &message, int EXP1, int EXP2)
{
    // mục tiêu chính
    const string PRIMARY_TARGETS[5] = {
        "Buon Ma Thuot",
        "Duc Lap",
        "Dak Lak",
        "National Route 21",
        "National Route 14"};

    // 1) lọc các ký tự hợp lệ
    string filtered;
    filterRec(message, 0, filtered);
    if (filtered.empty())
    {
        return "INVALID"; // chuỗi rỗng sau khi lọc
    }

    // 2) chuyển đổi thành Title Case
    titleCaseRec(filtered, 0, true);

    // 3) kiểm tra nếu filtered là một mục tiêu chính
    if (checkPrimaryTargetRec(filtered, PRIMARY_TARGETS, 0))
    {
        return filtered; // đã tìm thấy mục tiêu
    }

    // 4) giải mã
    string decoded;
    if (EXP1 >= 300 && EXP2 >= 300)
    {
        // Mã hóa Caesar
        decoded = filtered;
        int shift = (EXP1 + EXP2) % 26;
        caesarShiftRec(decoded, 0, shift); // áp dụng phép dịch Caesar
    }
    else
    {
        // Đảo ngược chuỗi
        decoded = filtered;
        if (!decoded.empty())
        {
            reverseStringRec(decoded, 0, decoded.size() - 1); // đảo ngược chuỗi
        }
    }

    // 5) chuyển đổi thành Title Case lại
    titleCaseRec(decoded, 0, true);

    // 6) kiểm tra kết quả cuối cùng
    if (checkPrimaryTargetRec(decoded, PRIMARY_TARGETS, 0))
    {
        return decoded; // đã tìm thấy mục tiêu
    }
    return "INVALID"; // không hợp lệ
}

// 5) manageLogistics - Quản lý hậu cần

// Định nghĩa 5 hàm tĩnh, mỗi hàm xử lý một loại E. Sau đó lưu chúng trong một mảng.
static void manageBucket0(int LF1, int LF2, int EXP1, int EXP2, int &T1, int &T2, int E)
{
    int total = T1 + T2;                                                    // tổng nguồn lực
    double fraction = ((LF1 + LF2) == 0) ? 0.5 : double(LF1) / (LF1 + LF2); // tỷ lệ phân phối
    double expDiff = 1.0 + double(EXP1 - EXP2) / 100.0;                     // điều chỉnh dựa trên kinh nghiệm
    int deltaT1 = myCeil(fraction * total * expDiff);                       // phần giảm của T1
    int deltaT2 = total - deltaT1;                                          // phần giảm của T2
    T1 = myMax(0, myMin(3000, T1 - deltaT1));                               // cập nhật T1
    T2 = myMax(0, myMin(3000, T2 - deltaT2));                               // cập nhật T2
}

static void manageBucket1(int LF1, int LF2, int EXP1, int EXP2, int &T1, int &T2, int E)
{
    double dT1 = T1 - T1 * (E / 100.0); // giảm T1 theo E
    double dT2 = T2 - T2 * (E / 200.0); // giảm T2 theo E (ít hơn)
    T1 = myCeil(dT1);                   // làm tròn lên
    T2 = myCeil(dT2);                   // làm tròn lên
}

static void manageBucket2(int LF1, int LF2, int EXP1, int EXP2, int &T1, int &T2, int E)
{
    int extra = E * 50;           // tính lượng tăng thêm dựa trên E
    T1 = myMin(3000, T1 + extra); // tăng T1 nhưng không quá 3000
    T2 = myMin(3000, T2 + extra); // tăng T2 nhưng không quá 3000
}

static void manageBucket3(int LF1, int LF2, int EXP1, int EXP2, int &T1, int &T2, int E)
{
    double inc1 = T1 * (E / 200.0);      // tính lượng tăng cho T1 dựa trên E
    double inc2 = T2 * (E / 500.0);      // tính lượng tăng cho T2 dựa trên E (ít hơn)
    T1 = myMin(3000, T1 + myCeil(inc1)); // tăng T1 và làm tròn lên
    T2 = myMin(3000, T2 + myCeil(inc2)); // tăng T2 và làm tròn lên
}

static void manageBucket4(int LF1, int LF2, int EXP1, int EXP2, int &T1, int &T2, int E)
{
    // không làm gì - giữ nguyên T1 và T2
}

// Lưu trữ các hàm trên trong một mảng con trỏ hàm
typedef void (*ManageFunc)(int, int, int, int, int &, int &, int); // khai báo kiểu con trỏ hàm

void manageLogistics(int LF1, int LF2, int EXP1, int EXP2, int &T1, int &T2, int E)
{
    // chọn bucket dựa vào giá trị của E
    int bucket;
    if (E == 0)
    {
        bucket = 0;
    }
    else if (E <= 9)
    {
        bucket = 1;
    }
    else if (E <= 29)
    {
        bucket = 2;
    }
    else if (E <= 59)
    {
        bucket = 3;
    }
    else
    {
        bucket = 4;
    }

    // Mảng các con trỏ hàm, mỗi phần tử trỏ đến một hàm xử lý bucket tương ứng
    static ManageFunc table[5] = {
        &manageBucket0,
        &manageBucket1,
        &manageBucket2,
        &manageBucket3,
        &manageBucket4};

    // Gọi hàm xử lý tương ứng với bucket đã chọn
    table[bucket](LF1, LF2, EXP1, EXP2, T1, T2, E);

    // kiểm tra và giới hạn giá trị sau khi xử lý
    if (T1 < 0)
    {
        T1 = 0; // đảm bảo T1 không âm
    }
    else if (T1 > 3000)
    {
        T1 = 3000; // đảm bảo T1 không vượt quá 3000
    }
    if (T2 < 0)
    {
        T2 = 0; // đảm bảo T2 không âm
    }
    else if (T2 > 3000)
    {
        T2 = 3000; // đảm bảo T2 không vượt quá 3000
    }
}

// 6) planAttack - Lập kế hoạch tấn công
int planAttack(int LF1, int LF2, int EXP1, int EXP2, int T1, int T2, int battleField[10][10])
{
    // giới hạn tất cả các giá trị đầu vào
    if (LF1 < 0)
    {
        LF1 = 0; // đảm bảo LF1 không âm
    }
    if (LF2 < 0)
    {
        LF2 = 0; // đảm bảo LF2 không âm
    }
    LF1 = clampInt(LF1, 0, 9999999); // giới hạn LF1
    LF2 = clampInt(LF2, 0, 9999999); // giới hạn LF2
    EXP1 = clampInt(EXP1, 0, 600);   // giới hạn EXP1
    EXP2 = clampInt(EXP2, 0, 600);   // giới hạn EXP2
    T1 = clampInt(T1, 0, 3000);      // giới hạn T1
    T2 = clampInt(T2, 0, 3000);      // giới hạn T2

    // Tính giá trị ban đầu S dựa trên tổng của các tham số
    double S = (LF1 + LF2) + (EXP1 + EXP2) * 5.0 + (T1 + T2) * 2.0;
    // Tính toán giá trị cuối cùng bằng cách xem xét mảng chiến trường
    double finalVal = planAttackRec(battleField, 0, 0, S);
    return myCeil(finalVal); // làm tròn lên và trả về kết quả
}

// 7) resupply - Tính toán lượng tiếp tế tối thiểu cần thiết
int resupply(int shortfall, int supply[5][5])
{
    int arr[25]; // mảng 1 chiều để lưu trữ tất cả các giá trị từ mảng 2 chiều
    {
        int idx = 0;
        flattenRec(supply, arr, 0, 0, idx); // chuyển đổi mảng 2D thành mảng 1D
    }
    insertionSortRec(arr, 25, 1); // sắp xếp mảng theo thứ tự tăng dần

    // Tìm tổ hợp 5 phần tử có tổng nhỏ nhất và lớn hơn hoặc bằng shortfall
    int minSum = 2147483647;          // giá trị lớn nhất của int
    loopA(arr, shortfall, minSum, 0); // bắt đầu tìm kiếm từ chỉ số 0
    if (minSum == 2147483647)
    {
        return -1; // không tìm thấy tổ hợp thỏa mãn
    }
    return minSum; // trả về tổng nhỏ nhất tìm được
}