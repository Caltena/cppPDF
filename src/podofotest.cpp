
#include "podofotest.hpp"

// ${fileDirname}

void fopodo::test()
{
    PdfMemDocument pdfDoc;
    pdfDoc.Load("template.pdf");

    PdfObject *rootObj = pdfDoc.GetTrailer()->GetIndirectKey("Root");
    PdfObject *acroForm = nullptr;
    if (rootObj && rootObj->IsDictionary())
    {
        PdfDictionary &rootDict = rootObj->GetDictionary();
        const PdfReference &acroFormRef = rootDict.GetKey("AcroForm")->GetReference();
        PdfObject *acroFormObj = pdfDoc.GetObjects().GetObject(acroFormRef);
        acroForm = acroFormObj;
    }

    if (acroForm && acroForm->IsDictionary())
    {
        PdfArray &fields = acroForm->GetDictionary().GetKey("Fields")->GetArray();

        for (size_t i = 0; i < fields.GetSize(); ++i)
        {
            PdfObject *fieldObj = pdfDoc.GetObjects().GetObject(fields[i].GetReference());
            PdfDictionary &dict = fieldObj->GetDictionary();

            PdfName fieldType = dict.GetKey("FT")->GetName();
            PdfString fieldName = dict.GetKey("T")->GetString();

            std::string a = fieldName.GetString();
            try
            {
                if (a == "name")
                {
                    dict.AddKey("V", PdfString("John Doe"));
                }
                if (a == "CheckBox")
                {
                    dict.AddKey("V", PdfString("No"));
                }
            }
            catch (const std::runtime_error &e)
            {
                std::cerr << "Fehler: " << e.what() << std::endl;
            }
        }
    }

    pdfDoc.Write("filled_form.pdf");
}